//
// Created by Alberto Campi on 2019-08-02.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
//

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "Utils.h"
#include <cmath>
#include <random>
#include <ctime>
#include <list>
#include <iterator>
#include "../Common/Math/Statistics/Stat.h"
#include "../Common/Math/Relative/RelativeModel.h"
#include "../Common/Math/MLRegression/RegressionModel.h"
#include "../Common/Math/Interpolation/Interpolator.h"
#include "../Common/Math/Interpolation/LinearInterpolator.h"
#include "../Common/Math/Interpolation/NaturalCubicSplineInterpolator.h"
#include "../Common/Types/DataSet.h"
#include "../Common/Utils/IO/JSONParser.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

const std::string expectedOutputRelativePath = "../UnitTests/Outputs/Expected/";
const std::string inputRelativePath = "../UnitTests/Inputs/";

BOOST_AUTO_TEST_SUITE(Statistics)
    const double tol = 0.1;
    BOOST_AUTO_TEST_CASE(MultivariateStat_Mean_Variance_Correlation_HappyPath, *utf::tolerance(tol))
    {
        const int statisticsDimension = 3;
        Math::MultivariateStat s(statisticsDimension);

        static std::mt19937 x(1), y(2), z(3); //use different seeds to reduce serial correlation
        std::normal_distribution<double> N52(5.0,2.0); //mean = 5, stdDev = 2
        std::normal_distribution<double> N01(0,1.0); //mean = 0, stdDev = 1
        std::normal_distribution<double> N11(11.5,7.2); //mean = 11.5, stdDev = 7.2

        const int NTrials = 1000;

        for (unsigned int i = 0; i < NTrials; ++i)
        {
            std::vector<double> rv = { N52(x), N01(y), N11(z) };
            s.add(rv);
        }

        std::vector<double> expMean = {5.0, 0.0, 11.5}, expVariance = {2.0*2.0, 1.0, 7.2*7.2},
                expStdDev = {2.0, 1.0, 7.2}, expStdErr = {2.0/sqrt(NTrials), 1.0/sqrt(NTrials), 7.2/sqrt(NTrials)};

        const std::vector<std::vector<double>> expCovContainer = {{2.0*2.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0 , 7.2*7.2}};
        const std::vector<std::vector<double>> expCorrContainer = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0 , 1.0}};

        BOOST_TEST(s.mean() == expMean, tt::per_element());
        BOOST_TEST(s.variance() == expVariance, tt::per_element());
        BOOST_TEST(s.stdDev() == expStdDev, tt::per_element());
        BOOST_TEST(s.stdError() == expStdErr, tt::per_element());

        const boost::numeric::ublas::matrix<double> actualCov = s.covariance();
        const boost::numeric::ublas::matrix<double> actualCorr = s.correlation();
        for (unsigned int i = 0; i < statisticsDimension; ++i)
        {
            BOOST_TEST(boost::numeric::ublas::row(actualCov, i) == expCovContainer[i], tt::per_element());
            BOOST_TEST(boost::numeric::ublas::row(actualCorr, i) == expCorrContainer[i], tt::per_element());
        }
    }

    BOOST_AUTO_TEST_CASE(UnivariateStat_Mean_Variance_HappyPath, *utf::tolerance(tol))
    {
        Math::UnivariateStat sod;
        static std::mt19937 x;
        std::normal_distribution<double> N11(11.5,7.2); //mean = 5, stdDev = 2
        const int NTrials = 1000;

        for (unsigned int i = 0; i < NTrials; ++i)
        {
            sod.add(N11(x));
        }

        BOOST_TEST(sod.mean() == 11.5);
        BOOST_TEST(sod.stdDev() == 7.2);
        BOOST_TEST(sod.variance() == 7.2*7.2);
        BOOST_TEST(sod.stdError() == 7.2/sqrt(NTrials));
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Relative)
    BOOST_AUTO_TEST_CASE(RelativeVolatilityModel_happyPath)
    {
        const std::vector<double> lhsData = {0.1, 0.15, 0.24, 0.12, 0.08, 0.095, 0.34, 0.29};
        const std::vector<double> rhsData = {1.4, 1.56, 2, 2.34, 5.6, 3};

        Math::UnivariateStat lhsS, rhsS;
        for (const auto& it: lhsData)
            lhsS.add(it);

        for (const auto& it: rhsData)
            rhsS.add(it);

        const double expectedParameter = rhsS.stdDev() / lhsS.stdDev();
        double coeff;
        Math::RelativeVolatilityModel mdl;
        mdl.calibrate(coeff, lhsData, rhsData);
        BOOST_CHECK_EQUAL(coeff, expectedParameter);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Interpolation)
    using namespace Math;

    BOOST_AUTO_TEST_CASE(Natural_cubic_spline_fit)
    {
        std::map<double, double> f = {{0, exp(0)}, {1, exp(1)}, {2, exp(2)}, {3, exp(3)}};

        NaturalCubicSplineInterpolator ncsi;
        ncsi.fitSpline(f);

        std::vector<CSCoeffs> expectedCoeffs = {{1, 1.4659976141747231, 0 , 0.25228421428432202},
                                                {2.7182818284590451, 2.2228502570276891, 0.75685264285296605, 1.6910713705909506},
                                                {7.3890560989306504, 8.8097696545064732, 5.8300667546258174, -1.9433555848752724},
                                                {20.085536923187668, 0, 0, 0}};

        for (unsigned int i = 0; i < 4; ++i)
            BOOST_TEST(ncsi.getCoefficients().at(i).operator==(expectedCoeffs.at(i)));
    }

    BOOST_AUTO_TEST_CASE(Natural_cubic_spline_interpolation, *utf::tolerance(1e-4))
    {
        std::map<double, double> f = {{0, exp(0)}, {1, exp(1)}, {2, exp(2)}, {3, exp(3)}};

        NaturalCubicSplineInterpolator ncsi;
        std::set<double> queryPoints;

        double sum = -1, rightEndPoint = 4;
        const double dx = 0.2;
        while (sum < rightEndPoint + dx)
        {
            queryPoints.insert(sum);
            sum += dx;
        }

        std::map<double, double> act = ncsi.interpolatePoints(f, queryPoints);

        const std::string expected = expectedOutputRelativePath + "Natural_cubic_spline_interpolation_expected.txt";
        std::ifstream expectedIn;
        expectedIn.open(expected);
        std::map<double, double> exp;
        readMap(exp, expectedIn, false);
        expectedIn.close();

        //Extract values from map to perform element-wise comparison with tolerance decorator
        //IMPORTANT: tolerance would be ignored by BOOST_TEST as maps do not have overloads for order relationship operators
        std::vector<double> actVals, expVals;
        std::map<double, double>::iterator it = act.begin(), itt = exp.begin();
        for (; it != act.end() and itt != exp.end(); ++it, ++itt)
            actVals.push_back(it -> second), expVals.push_back(itt -> second);

        BOOST_TEST(actVals == expVals, tt::per_element());
    }

    BOOST_AUTO_TEST_CASE(Linear_interpolation)
    {
        const std::pair<double, double> a = std::make_pair(-1, -1); //y = x^3
        const std::pair<double, double> b = std::make_pair(-2, -8);
        const std::pair<double, double> c = std::make_pair(0, 0);
        const std::pair<double, double> d = std::make_pair(1, 1);
        const std::pair<double, double> e = std::make_pair(2, 8);
        const std::map<double, double > f = {a, b, c, d, e}; //fill in map with some function

        LinearInterpolator lin;
        const std::map<double, double> expected = {{-1.5, -9./2.}, {0., 0.}, {-3., -15.}, {3., 15.}};
        const std::set<double> x = {-1.5, 0, -3, 3};
        //const double expectedInterpolate = -9./2.;

        BOOST_CHECK(lin.interpolatePoints(f, x) == expected);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(MovingAverage)
    void loadData(std::vector<double>& data, const std::string& filePath, bool header)
    {
        std::ifstream in;
        in.open(filePath);

        if (!in.is_open())
            throw std::runtime_error("MovingAverage : impossible to open file" + filePath);

        readArray(data, in, header);
        in.close();
    }
/*
    void dropNaN(std::list<double>& dataWithNaNs, unsigned int chunkSize)
    {
        std::list<double>::iterator it, jt;
        it = jt = dataWithNaNs.begin();
        std::advance(jt, chunkSize);
        dataWithNaNs.erase(it, jt);
    }
*/
    BOOST_AUTO_TEST_CASE(MA_happyPath, *utf::tolerance(1e-3))
    {
        const std::string fileName = "MA_data_test.txt";
        std::vector<double> data;
        loadData(data, inputRelativePath + fileName, true);

        const unsigned int windowSize = 4;
        Math::MovingAverage ma(windowSize);

        std::vector<double> maData;
        for (const auto& value : data)
        {
            ma.add(value);
            if (!std::isnan(ma.get()))
                maData.push_back(ma.get());
        }

        const std::string expectedFileName = "MA_happyPath_expected.txt";
        std::vector<double> expectedMAData;
        loadData(expectedMAData, expectedOutputRelativePath + expectedFileName, true);

        BOOST_TEST(maData == expectedMAData, tt::per_element());
    }

    BOOST_AUTO_TEST_CASE(CMA_happyPath, *utf::tolerance(1e-3))
    {
        const std::string fileName = "MA_data_test.txt";
        std::vector<double> data;
        loadData(data, inputRelativePath + fileName, true);

        const unsigned int windowSize = 4;
        Math::CenteredMovingAverage cma(windowSize);

        std::vector<double> maData;
        for (const auto& value : data)
        {
            cma.add(value);
            if (!std::isnan(cma.get()))
                maData.push_back(cma.get());
            //std::cout << cma.get() << std::endl;
        }

        const std::string expectedFileName = "CMA_happyPath_expected.txt";
        std::vector<double> expectedCMAData;
        loadData(expectedCMAData, expectedOutputRelativePath + expectedFileName, true);

        BOOST_TEST(maData == expectedCMAData, tt::per_element());
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MLRegression)

    // DataSet loader from json file
    void loadDataSet(const std::string& fullFileName, Common::DataSet& ds)
    {
        Common::JSONParserDecoratorDataSet js;
        js.readJSON(fullFileName);
        ds = js.getDataSet();
    }

    // Percentage values calculator
    double percentageValuesCalc (const Common::DataSet& ds, const std::string& variableName, unsigned int index)
    {
        const double percentageValue =
                (ds.getTimeSeries(variableName).getValues().at(index) / ds.getTimeSeries(variableName).getValues().at(index - 1)) - 1;

        return  percentageValue;
    }

    // Fixture structure for vector and matrix preload for regression tests
    struct FxInputData
    {
        FxInputData(const std::string &dVarName,
                    const std::vector<std::string>& idVarNames,
                    const std::string& fileName) :
                        m_idVarNames(idVarNames), m_dVarName(dVarName), m_fileName(fileName)
        {
            loadDataSet(inputRelativePath + fileName, m_ds);

            boost::gregorian::date firstValidDate = m_ds.getTimeSeries(m_dVarName).getDates().at(0);
            for (const auto& var : m_idVarNames)
            {
                if (m_ds.getTimeSeries(var).getDates().at(0) > firstValidDate)
                    firstValidDate = m_ds.getTimeSeries(var).getDates().at(0);
            }

            // Dependent variables vector loader with percentage values
            const unsigned int indexY = m_ds.getTimeSeries(m_dVarName).getIndex(firstValidDate);
            boost::numeric::ublas::vector<double> vectorY(m_ds.getTimeSeries(m_dVarName).length() - indexY - 1);
            unsigned j = 0;
            for (unsigned int i = indexY + 1; i < m_ds.getTimeSeries(m_dVarName).length(); ++i)
            {
                vectorY(j) = percentageValuesCalc(m_ds, m_dVarName, i);
                ++j;
            }

            // Independent values matrix loader with percentage values
            boost::numeric::ublas::matrix<double> matrixXn(vectorY.size(), m_idVarNames.size() + 1, 1);
            unsigned int column = 0;
            for (auto it = m_idVarNames.cbegin(); it < m_idVarNames.cend(); ++it)
            {
                unsigned int index = m_ds.getTimeSeries(*it).getIndex(firstValidDate);
                for (unsigned int i = 0; i < vectorY.size(); ++i)
                {
                    matrixXn(i, column) = percentageValuesCalc(m_ds, *it, index + 1);
                    ++index;
                }
                ++column;
            }

            m_dVar = vectorY;
            m_idVars = matrixXn;
        }

        Common::DataSet m_ds;
        std::string m_fileName, m_dVarName;
        std::vector<std::string> m_idVarNames;
        boost::numeric::ublas::vector<double> m_dVar;
        boost::numeric::ublas::matrix<double> m_idVars;
    };
/*
    // Matrix reader
    void readMatrix (const boost::numeric::ublas::matrix<double>& matrix)
    {
        for (int i = 0; i < matrix.size1(); ++i)
        {
            for (int j = 0; j < matrix.size2(); ++j)
            {
                std::cout << matrix(i, j) << "\t";
            }
            std::cout << std::endl;
        }
    }

    BOOST_AUTO_TEST_CASE(MatrixDeterminantTest)
    {
        boost::numeric::ublas::matrix<double> inputMatrix(3, 3);

        inputMatrix(0, 0) = 1.; inputMatrix(0, 1) = 1.; inputMatrix(0, 2) = 1.;
        inputMatrix(1, 0) = 2.; inputMatrix(1, 1) = -1.; inputMatrix(1, 2) = 1.;
        inputMatrix(2, 0) = 1.; inputMatrix(2, 1) = 1.; inputMatrix(2, 2) = 2.;

        double matrixDeterminant(Math::computeMatrixDeterminant(inputMatrix));

        double expectedDeterminant = -3.;

        BOOST_TEST(matrixDeterminant == expectedDeterminant);
    }

    BOOST_AUTO_TEST_CASE(InversionMatrixTest)
    {
        boost::numeric::ublas::matrix<double> inputMatrix(3, 3);

        inputMatrix(0, 0) = 1.; inputMatrix(0, 1) = 3.; inputMatrix(0, 2) = 3.;
        inputMatrix(1, 0) = 1.; inputMatrix(1, 1) = 4.; inputMatrix(1, 2) = 3.;
        inputMatrix(2, 0) = 1.; inputMatrix(2, 1) = 3.; inputMatrix(2, 2) = 4.;

        boost::numeric::ublas::matrix<double> invertedMatrix(Math::computeInverseMatrix(inputMatrix));
        boost::numeric::ublas::matrix<double> expectedMatrix(3, 3);

        expectedMatrix(0, 0) = 7.; expectedMatrix(0, 1) = -3.; expectedMatrix(0, 2) = -3.;
        expectedMatrix(1, 0) = -1.; expectedMatrix(1, 1) = 1.; expectedMatrix(1, 2) = 0.;
        expectedMatrix(2, 0) = -1.; expectedMatrix(2, 1) = 0.; expectedMatrix(2, 2) = 1.;

        for (unsigned int i = 0; i < expectedMatrix.size1(); ++i)
            BOOST_TEST(boost::numeric::ublas::row(invertedMatrix, i) == boost::numeric::ublas::row(expectedMatrix, i),
                       tt::per_element());
    }
*/
    BOOST_AUTO_TEST_CASE(CholeskyDecompositionTest, *utf::tolerance(1e-4))
    {
        const int dim = 4;
        boost::numeric::ublas::matrix<double> inputMatrix(dim, dim);

        inputMatrix(0, 0) = 18.; inputMatrix(0, 1) = 22.; inputMatrix(0, 2) = 54.; inputMatrix(0, 3) = 42.;
        inputMatrix(1, 0) = 22.; inputMatrix(1, 1) = 70.; inputMatrix(1, 2) = 86.; inputMatrix(1, 3) = 62.;
        inputMatrix(2, 0) = 54.; inputMatrix(2, 1) = 86.; inputMatrix(2, 2) = 174.; inputMatrix(2, 3) = 134.;
        inputMatrix(3, 0) = 42.; inputMatrix(3, 1) = 62.; inputMatrix(3, 2) = 134.; inputMatrix(3, 3) = 106.;

        /*std::cout << "The input matrix is:" << std::endl;
        readMatrix(inputMatrix);*/

        //boost::numeric::ublas::matrix<double> factorizedMatrix(Math::choleskyDecompose(inputMatrix));
        Math::CholeskyDecompose ch;
        ch.decompose(inputMatrix);
        const boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> factorizedMatrix = ch.getCholeskyFactor();

        /*std::cout << "The factorized matrix is:" << std::endl;
        readMatrix(factorizedMatrix);*/

        boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> expectedMatrix(dim, dim);

        expectedMatrix(0, 0) = 4.24264;
        expectedMatrix(1, 0) = 5.18545; expectedMatrix(1, 1) = 6.56591;
        expectedMatrix(2, 0) = 12.72792; expectedMatrix(2, 1) = 3.04604; expectedMatrix(2, 2) = 1.64974;
        expectedMatrix(3, 0) = 9.89949; expectedMatrix(3, 1) = 1.62455; expectedMatrix(3, 2) = 1.84971; expectedMatrix(3, 3) = 1.39262;

        for (unsigned int i = 0; i < expectedMatrix.size1(); ++i)
            BOOST_TEST(boost::numeric::ublas::row(factorizedMatrix, i) == boost::numeric::ublas::row(expectedMatrix, i),
                       tt::per_element());
    }

    BOOST_AUTO_TEST_CASE(MoorePenroseRegressionTest, *utf::tolerance(1e-4))
    {
        const std::string fileName = "sample_dataSet_clean.json";
        const std::string dVarName = "HANG_SENG";
        const std::vector<std::string> idVarNames = {"DOW_JONES", "US_GDP_SAAR"};
        FxInputData fx(dVarName, idVarNames, fileName);
        const boost::numeric::ublas::vector<double> vectorY = fx.m_dVar;
        boost::numeric::ublas::matrix<double> independentValuesMatrix = fx.m_idVars;
        boost::numeric::ublas::vector<double> coefficients(independentValuesMatrix.size2());

        clock_t startTime = clock();

        Math::RegressionModelAlgorithmMoorePenrose mpRegression;
        mpRegression.calibrate(coefficients, vectorY, independentValuesMatrix);

        clock_t endTime = clock();
        double executionTime = double (endTime - startTime) / double (CLOCKS_PER_SEC);

        std::cout << "Execution time in: " << executionTime << " seconds" << std::endl;

        std::vector<double> targetCoefficients;
        targetCoefficients.push_back(1.0957);
        targetCoefficients.push_back(1.9296);
        targetCoefficients.push_back(-0.021125);

        BOOST_TEST(coefficients == targetCoefficients, tt::per_element());

        const double residualMSE = 0.008797539080674183;
        const boost::numeric::ublas::matrix<double> covMatrix = mpRegression.computeCoefficientCovarianceMatrix(residualMSE);

        const std::vector<double> expectedStdErrs = {0.13136823, 1.29721688, 0.01689724};

        for (unsigned int i = 0; i < covMatrix.size1(); ++i)
            BOOST_TEST(sqrt(covMatrix(i, i)) == expectedStdErrs[i]);

    }

    BOOST_AUTO_TEST_CASE(CholeskyRegressionTest, *utf::tolerance(1e-4))
    {
        const std::string fileName = "sample_dataSet_clean.json";
        const std::string dVarName = "HANG_SENG";
        const std::vector<std::string> idVarNames = {"DOW_JONES", "US_GDP_SAAR"};
        FxInputData fx(dVarName, idVarNames, fileName);
        const boost::numeric::ublas::vector<double> vectorY = fx.m_dVar;
        const boost::numeric::ublas::matrix<double> independentValuesMatrix = fx.m_idVars;
        boost::numeric::ublas::vector<double> coefficients(independentValuesMatrix.size2());

        clock_t startTime = clock();

        Math::RegressionModelOLS chlRegression;
        chlRegression.calibrate(coefficients, vectorY, independentValuesMatrix);

        clock_t endTime = clock();
        double executionTime = double (endTime - startTime) / double (CLOCKS_PER_SEC);

        std::cout << "Execution time in: " << executionTime << " seconds" << std::endl;

        std::vector<double> targetCoefficients;
        targetCoefficients.push_back(1.0957);
        targetCoefficients.push_back(1.9296);
        targetCoefficients.push_back(-0.021125);

        BOOST_TEST(coefficients == targetCoefficients, tt::per_element());

        Math::ANOVASummary summary = chlRegression.getANOVA();

        const double expectedTotalMSE = 0.014261017562460713;
        const double expectedModelMSE = 0.36119190115590566;
        const double expectedResidualMSE = 0.008797539080674183;
        const double expectedRSquared = 0.39267004700044816;
        const double expectedAdjRSquared = 0.3831057957721088;

        BOOST_TEST(summary.totalMSEVariance == expectedTotalMSE);
        BOOST_TEST(summary.modelMSEVariance == expectedModelMSE);
        BOOST_TEST(summary.residualMSEVariance == expectedResidualMSE);
        BOOST_TEST(summary.RSquared == expectedRSquared);
        BOOST_TEST(summary.adjRSquared == expectedAdjRSquared);
    }

BOOST_AUTO_TEST_SUITE_END()