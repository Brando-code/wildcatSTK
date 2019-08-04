//
// Created by Alberto Campi on 2019-08-02.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
//

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "../wildcatSTKCoreIncludes.h"

#include <cmath>
#include <random>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

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
            for (unsigned int j = 0; j < statisticsDimension; ++j)
            {
                BOOST_TEST(actualCov(i, j) == expCovContainer[i][j]);
                BOOST_TEST(actualCorr(i, j) == expCorrContainer[i][j]);
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
        std::vector<double> coeffs;
        Math::RelativeVolatilityModel mdl;
        mdl.calibrate(coeffs, lhsData, rhsData);
        BOOST_CHECK_EQUAL(coeffs.size(), 1);
        BOOST_CHECK_EQUAL(coeffs[0], expectedParameter);
    }

BOOST_AUTO_TEST_SUITE_END()