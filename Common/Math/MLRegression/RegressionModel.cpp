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
    const boost::numeric::ublas::vector<double> XtY = boost::numeric::ublas::prod(Xt, dependentVariableValues);

    coefficients = boost::numeric::ublas::prod(m_invXtX, XtY);
    m_coefficients = coefficients;
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
    const boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> choleskyFactor = m_ch.getCholeskyFactor();
    const long dim = choleskyFactor.size1();

    // solve lower triangular system Lw=A*b for w by forward substitution (* = transposition)
    const boost::numeric::ublas::vector<double> XtY(boost::numeric::ublas::prod(Xt, dependentVariableValues));

    boost::numeric::ublas::vector<double> omega(choleskyFactor.size2());
    for (unsigned long i = 0; i < dim; ++i)
    {
        double sum = 0;
        for (unsigned long j = 0; j < i; ++j)
        {
            sum += choleskyFactor(i, j) * omega(j);
        }
        omega(i) = (XtY(i) - sum) / choleskyFactor(i,i);
    }

    // solve upper triangular system L*x=w for x by backward substitution
    for (long i = dim - 1; i >= 0 ; --i)
    {
        double sum = 0;
        for (unsigned long j = i + 1; j < dim ; ++j)
        {
            sum += choleskyFactor(j, i) * coefficients(j);
        }
        coefficients(i) = (omega(i) - sum) / choleskyFactor(i,i);
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