//
// Created by Alberto Campi on 2019-08-03.
//


#include "RegressionModel.h"
#include "../Statistics/Stat.h"
#include <boost/numeric/ublas/lu.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/mat_traits_array.hpp>
#include <boost/math/distributions/students_t.hpp>

/* AUXILIARY FUNCTIONS
 *--------------------------------------------------------------------------------------------------------------------*/

// Matrix inversion
boost::numeric::ublas::matrix<double> Math::computeInverseMatrix (boost::numeric::ublas::matrix<double> inputMatrix)
{
    boost::numeric::ublas::matrix<double> inverseMatrix = boost::numeric::ublas::identity_matrix<double>(inputMatrix.size1());
    boost::numeric::ublas::permutation_matrix<size_t> permutationMatrix(inputMatrix.size1());
    lu_factorize(inputMatrix, permutationMatrix);
    lu_substitute(inputMatrix, permutationMatrix, inverseMatrix);

    return inverseMatrix;
}

// Matrix determinant sign
int Math::computeDeterminantSign(const boost::numeric::ublas::permutation_matrix<std::size_t> &permutationMatrix)
{
    int detSign = 1.;
    for (int i = 0; i < permutationMatrix.size(); ++i)
    {
        if (i != permutationMatrix(i))
            detSign *= -1.;
    }
    return detSign;
}

// Matrix determinant
double Math::computeMatrixDeterminant(boost::numeric::ublas::matrix<double> inputMatrix)
{
    boost::numeric::ublas::permutation_matrix<std::size_t> permutationMatrix(inputMatrix.size1());

    double determinant = 1.;
    if (boost::numeric::ublas::lu_factorize(inputMatrix, permutationMatrix))
    {
        determinant = 0.;
    }
    else
    {
        for (int i = 0; i < inputMatrix.size1(); ++i)
        {
            determinant *= inputMatrix(i, i);
        }
        determinant *= computeDeterminantSign(permutationMatrix);
    }
    return determinant;
}

// Cholesky Decomposition
boost::numeric::ublas::matrix<double> Math::choleskyDecompose(const boost::numeric::ublas::matrix<double> &inputMatrix)
{
    const int dim = inputMatrix.size1();
    boost::numeric::ublas::matrix<double> outputMatrix(boost::numeric::ublas::zero_matrix<double>(dim, dim));

    for (unsigned int i = 0; i < dim ; ++i)
    {
        for (unsigned int k = 0; k < i + 1; ++k)
        {
            double sum = 0;
            for (unsigned int j = 0; j < k; ++j)
            {
                sum += outputMatrix(i,j) * outputMatrix(k,j);
            }

            outputMatrix(i,k) = (i == k) ? sqrt(inputMatrix(i,i) - sum) :
                                                        ((1./outputMatrix(k,k)) * (inputMatrix(i,k) - sum));
        }
    }
    return outputMatrix;
}


/*REGRESSION MODEL METHODS
 * --------------------------------------------------------------------------------------------------------*/
Math::ANOVASummary Math::ANOVA::computeANOVA(const boost::numeric::ublas::vector<double> &coefficients,
                                             const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                             const boost::numeric::ublas::matrix<double> &independentVariableValues,
                                             const Math::RegressionModelAlgorithm &reg) const
{
    Math::ANOVASummary rv;

    //Overall regression diagnostics
    Math::UnivariateStat acc;
    for (const auto& value : dependentVariableValues)
        acc.add(value);

    rv.totalMean = acc.mean();
    rv.sampleSize = dependentVariableValues.size();
    rv.totalDoF = rv.sampleSize - 1;
    rv.totalMSEVariance = acc.variance() * dependentVariableValues.size() / rv.totalDoF;

    rv.fittedValues = boost::numeric::ublas::prod(independentVariableValues, coefficients);
    rv.residuals = dependentVariableValues - rv.fittedValues;
    rv.residualDoF = rv.sampleSize - independentVariableValues.size2();
    rv.residualMSEVariance = boost::numeric::ublas::inner_prod(rv.residuals, rv.residuals) / rv.residualDoF;

    const boost::numeric::ublas::vector<double> modelMeanDeviation = rv.fittedValues -
                                                             boost::numeric::ublas::vector<double>(rv.fittedValues.size(), rv.totalMean);

    rv.modelDoF = independentVariableValues.size2() - 1;
    rv.modelMSEVariance = boost::numeric::ublas::inner_prod(modelMeanDeviation, modelMeanDeviation) / rv.modelDoF;

    rv.adjRSquared = 1 - rv.residualMSEVariance / rv.totalMSEVariance;
    rv.RSquared = 1 - (rv.residualMSEVariance * rv.residualDoF) / (rv.totalMSEVariance * rv.totalDoF);

    //Estimated coefficient diagnostics
    const boost::numeric::ublas::matrix<double> coeffsCovMatrix = reg.computeCoefficientCovarianceMatrix(rv.residualMSEVariance);
    SummaryStatistic st;

    for (unsigned int i = 0; i < coeffsCovMatrix.size1(); ++i)
    {
        st.stdErr = sqrt(coeffsCovMatrix(i, i));
        st.tRatio = coefficients(i) / st.stdErr;
        st.pValue = boost::math::cdf(boost::math::students_t_distribution<double>(rv.sampleSize), -2 * std::abs(st.tRatio));
        rv.coefficientSummaryStat.push_back(st);
    }

    return rv;
}

Math::RegressionModelOLS::RegressionModelOLS() :
    m_algorithmPtr(Math::RegressionModelAlgorithmCholesky().clone())
{

}

Math::RegressionModelOLS::RegressionModelOLS(const Math::RegressionModelOLS &other) :
    m_algorithmPtr(other.m_algorithmPtr -> clone())
{

}

void Math::RegressionModelOLS::calibrate(boost::numeric::ublas::vector<double> &coefficients,
                                         const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                         const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    if (dependentVariableValues.size() == independentVariableValues.size1() and independentVariableValues.size1() > 2)
        m_algorithmPtr -> calibrate(coefficients, dependentVariableValues, independentVariableValues);
    else
        throw std::runtime_error("Math::RegressionModelOLS::calibrate : "
                                 "at least two observations are needed for regression model to run.");
}

Math::ANOVASummary Math::RegressionModelOLS::getANOVA() const
{
    return m_algorithmPtr -> getANOVA();
}

std::unique_ptr<Math::RegressionModel> Math::RegressionModelOLS::clone() const
{
    return std::make_unique<Math::RegressionModelOLS>(*this);
}


Math::ANOVASummary Math::RegressionModelAlgorithm::getANOVA() const
{
    Math::ANOVA anova;
    return anova.computeANOVA(m_coefficients, m_depVariableVals, m_indepVariableVals, *this);
}

//Regression by Moore-Penrose method
void Math::RegressionModelAlgorithmMoorePenrose::calibrate(boost::numeric::ublas::vector<double> &coefficients,
                                                           const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                           const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    m_depVariableVals = dependentVariableValues, m_indepVariableVals = independentVariableValues;

    const boost::numeric::ublas::matrix<double> transposedIndiVal(boost::numeric::ublas::trans(independentVariableValues));
    const boost::numeric::ublas::matrix<double> productTransIndiVal(boost::numeric::ublas::prod(transposedIndiVal,
                                                                                                independentVariableValues));
    // Inversion check
    const double determinantMatrix = Math::computeMatrixDeterminant(productTransIndiVal);
    if (determinantMatrix == 0.)
        throw std::runtime_error("Error: matrix is not invertible.");

    const boost::numeric::ublas::matrix<double> inverseProduct(computeInverseMatrix(productTransIndiVal));
    const boost::numeric::ublas::matrix<double> productInverseTransposed(boost::numeric::ublas::prod(inverseProduct,
                                                                                                     transposedIndiVal));

    coefficients = boost::numeric::ublas::prod(productInverseTransposed, dependentVariableValues);
    m_coefficients = coefficients;
}

boost::numeric::ublas::matrix<double> Math::RegressionModelAlgorithmMoorePenrose::computeCoefficientCovarianceMatrix(
        double residualVariance) const
{
    return residualVariance * Math::computeInverseMatrix(boost::numeric::ublas::prod(
            boost::numeric::ublas::trans(m_indepVariableVals), m_indepVariableVals));
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmMoorePenrose::clone() const
{
    return std::make_unique<RegressionModelAlgorithmMoorePenrose> (*this);
}

void Math::RegressionModelAlgorithmCholesky::calibrate(boost::numeric::ublas::vector<double> &coefficients,
                                                       const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    m_depVariableVals = dependentVariableValues, m_indepVariableVals = independentVariableValues;

    const boost::numeric::ublas::matrix<double> transposedIndiVal(boost::numeric::ublas::trans(independentVariableValues));
    const boost::numeric::ublas::matrix<double> productTransIndiVal(boost::numeric::ublas::prod(transposedIndiVal,
                                                                    independentVariableValues));
    const boost::numeric::ublas::matrix<double> choleskyFactor(Math::choleskyDecompose(productTransIndiVal));

    const int dim = choleskyFactor.size1();

    // solve lower triangular system Lw=A*b for w by forward substitution (* = transposition)
    const boost::numeric::ublas::vector<double> productAb(boost::numeric::ublas::prod(transposedIndiVal, dependentVariableValues));

    boost::numeric::ublas::vector<double> omegaVector(choleskyFactor.size2());
    for (unsigned int i = 0; i < dim; ++i)
    {
        double sum = 0;
        for (unsigned int j = 0; j < i; ++j)
        {
            sum += choleskyFactor(i, j) * omegaVector(j);
        }
        omegaVector(i) = (productAb(i) - sum) / choleskyFactor(i,i);
    }

    // solve upper triangular system L*x=w for x by backward substitution
    for (int i = dim - 1; i >= 0 ; --i)
    {
        double sum = 0;
        for (unsigned int j = i + 1; j < dim ; ++j)
        {
            sum += choleskyFactor(j, i) * coefficients(j);
        }
        coefficients(i) = (omegaVector(i) - sum) / choleskyFactor(i,i);
    }
    m_coefficients = coefficients;
}

boost::numeric::ublas::matrix<double> Math::RegressionModelAlgorithmCholesky::computeCoefficientCovarianceMatrix(
        double residualVariance) const
{
    //implement...
    return boost::numeric::ublas::matrix<double>();
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmCholesky::clone() const
{
    return std::make_unique<RegressionModelAlgorithmCholesky> (*this);
}