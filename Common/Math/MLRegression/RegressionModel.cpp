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
/*
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

*/
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

    for (unsigned long i = 0; i < coeffsCovMatrix.size1(); ++i)
    {
        st.stdErr = sqrt(coeffsCovMatrix(i, i));
        st.tRatio = coefficients(i) / st.stdErr;
        st.pValue = 2 * boost::math::cdf(complement(boost::math::students_t_distribution<double>(rv.sampleSize - 1), std::abs(st.tRatio)));
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
Math::RegressionModelAlgorithmMoorePenrose::RegressionModelAlgorithmMoorePenrose() : m_invXtX(), m_invertibleFlag(true)
{

}

void Math::RegressionModelAlgorithmMoorePenrose::_computeInverseByLUFactorization(boost::numeric::ublas::matrix<double> M,
                                                                                  boost::numeric::ublas::matrix<double> &M_inv) const
{
    M_inv = boost::numeric::ublas::identity_matrix<double>(M.size1());
    boost::numeric::ublas::permutation_matrix<size_t> permutationMatrix(M.size1());

    auto isSingular = lu_factorize(M, permutationMatrix);
    if (isSingular)
    {
        m_invertibleFlag = false;
        std::cerr << "Math::computeInverseMatrix : singular matrix is not invertible." << std::endl;
        return;
    }

    lu_substitute(M, permutationMatrix, M_inv);
}

void Math::RegressionModelAlgorithmMoorePenrose::calibrate(boost::numeric::ublas::vector<double> &coefficients,
                                                           const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                           const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    m_depVariableVals = dependentVariableValues, m_indepVariableVals = independentVariableValues;

    const boost::numeric::ublas::matrix<double> Xt(boost::numeric::ublas::trans(independentVariableValues));
    const boost::numeric::ublas::matrix<double> XtX(boost::numeric::ublas::prod(Xt, independentVariableValues));

    _computeInverseByLUFactorization(XtX, m_invXtX);
    if (!m_invertibleFlag)
        return;

    const boost::numeric::ublas::vector<double> XtY = boost::numeric::ublas::prod(Xt, dependentVariableValues);
    coefficients = boost::numeric::ublas::prod(m_invXtX, XtY);
    m_coefficients = coefficients;
}

bool Math::RegressionModelAlgorithmMoorePenrose::hasFailed() const
{
    return m_invertibleFlag;
}

boost::numeric::ublas::matrix<double> Math::RegressionModelAlgorithmMoorePenrose::computeCoefficientCovarianceMatrix(
        double residualVariance) const
{
    return residualVariance * m_invXtX;
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

    const boost::numeric::ublas::matrix<double> Xt(boost::numeric::ublas::trans(independentVariableValues));
    const boost::numeric::ublas::matrix<double> XtX(boost::numeric::ublas::prod(Xt, independentVariableValues));

    m_ch.decompose(XtX);
    if (m_ch.hasFailed())
        return;

    const boost::numeric::ublas::vector<double> XtY(boost::numeric::ublas::prod(Xt, dependentVariableValues));
    coefficients = _choleskySolve(m_ch.getCholeskyFactor(), XtY);
    m_coefficients = coefficients;
}

boost::numeric::ublas::vector<double> Math::RegressionModelAlgorithmCholesky::_choleskySolve(
        const boost::numeric::ublas::matrix<double> &choleskyFactor, const boost::numeric::ublas::vector<double> &rhs) const
{
    const long dim = choleskyFactor.size1();

    boost::numeric::ublas::vector<double> omega(dim);
    for (unsigned long i = 0; i < dim; ++i)
    {
        double sum = 0;
        for (unsigned long j = 0; j < i; ++j)
        {
            sum += choleskyFactor(i, j) * omega(j);
        }
        omega(i) = (rhs(i) - sum) / choleskyFactor(i, i);
    }

    // solve upper triangular system L*x=w for x by backward substitution
    boost::numeric::ublas::vector<double> x(dim);
    for (long i = dim - 1; i >= 0 ; --i)
    {
        double sum = 0;
        for (unsigned long j = i + 1; j < dim ; ++j)
        {
            sum += choleskyFactor(j, i) * x(j);
        }
        x(i) = (omega(i) - sum) / choleskyFactor(i, i);
    }
    return x;
}

boost::numeric::ublas::matrix<double> Math::RegressionModelAlgorithmCholesky::_choleskySolve(
        const boost::numeric::ublas::matrix<double> &choleskyFactor, const boost::numeric::ublas::matrix<double> &rhs) const
{
    const long dim = choleskyFactor.size1();

    boost::numeric::ublas::matrix<double> omega(dim, dim);
    for (unsigned long i = 0; i < dim; ++i)
    {
        boost::numeric::ublas::vector<double> sum(dim, 0);
        for (unsigned long j = 0; j < i; ++j)
        {
            for (unsigned long k = 0; k < dim; ++k)
                sum(k) += choleskyFactor(i, j) * omega(j, k);
        }
        boost::numeric::ublas::row(omega, i) = (boost::numeric::ublas::row(rhs, i) - sum) / choleskyFactor(i, i);
    }

    // solve upper triangular system L*x=w for x by backward substitution
    boost::numeric::ublas::matrix<double> x(dim, dim);
    for (long i = dim - 1; i >= 0 ; --i)
    {
        boost::numeric::ublas::vector<double> sum(dim, 0);
        for (unsigned long j = i + 1; j < dim ; ++j)
        {
            for (unsigned long k = 0; k < dim; ++k)
                sum(k) += choleskyFactor(j, i) * x(j, k);
        }
        boost::numeric::ublas::row(x, i) = (boost::numeric::ublas::row(omega, i) - sum) / choleskyFactor(i, i);
    }
    return x;
}

bool Math::RegressionModelAlgorithmCholesky::hasFailed() const
{
    return m_ch.hasFailed();
}

boost::numeric::ublas::matrix<double> Math::RegressionModelAlgorithmCholesky::computeCoefficientCovarianceMatrix(
        double residualVariance) const
{
    const boost::numeric::ublas::matrix<double> variance =
            residualVariance * boost::numeric::ublas::identity_matrix<double>(m_coefficients.size());
    return _choleskySolve(m_ch.getCholeskyFactor(), variance);
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmCholesky::clone() const
{
    return std::make_unique<RegressionModelAlgorithmCholesky> (*this);
}