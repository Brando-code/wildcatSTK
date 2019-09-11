//
// Created by Alberto Campi on 2019-08-03.
//


#include "RegressionModel.h"
#include "../Statistics/Stat.h"
#include <boost/numeric/ublas/lu.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/mat_traits_array.hpp>
#include <boost/math/distributions/students_t.hpp>


//
// ANOVA class implementation
//
Math::ANOVASummary Math::ANOVA::computeANOVA(const boost::numeric::ublas::vector<double> &coefficients,
                                             const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                             const boost::numeric::ublas::matrix<double> &independentVariableValues,
                                             const Math::RegressionModelAlgorithm &reg) const
{
    Math::ANOVASummary rv;

    // Overall regression diagnostics
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

    // Estimated coefficient diagnostics
    const boost::numeric::ublas::matrix<double> coeffsCovMatrix = reg.computeCoefficientCovarianceMatrix(rv.residualMSEVariance);
    Math::SummaryStatistic st;

    for (unsigned long i = 0; i < coeffsCovMatrix.size1(); ++i)
    {
        st.stdErr = sqrt(coeffsCovMatrix(i, i));
        st.tRatio = coefficients(i) / st.stdErr;
        st.pValue = 2 * boost::math::cdf(boost::math::students_t_distribution<double>(rv.sampleSize - 1), -std::abs(st.tRatio));
        rv.coefficientSummaryStat.push_back(st);
    }
    return rv;
}


//
// Regression model interface implementation for OLS sub-type
//
Math::RegressionModelOLS::RegressionModelOLS() :
    m_algorithmPtr(Math::RegressionModelAlgorithmCholesky().clone()) //[AC] initialize to default algorithm (first link in chain)
{

}

Math::RegressionModelOLS::RegressionModelOLS(const Math::RegressionModelOLS &other) :
    m_algorithmPtr(other.m_algorithmPtr -> clone())
{

}

Math::RegressionModelOLS& Math::RegressionModelOLS::operator=(const Math::RegressionModelOLS &other)
{
    if (&other != this)
        m_algorithmPtr = other.m_algorithmPtr -> clone();

    return *this;
}

void Math::RegressionModelOLS::calibrate(boost::numeric::ublas::vector<double> &coefficients,
                                         const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                         const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    if (dependentVariableValues.size() == independentVariableValues.size1() and independentVariableValues.size1() > 2)
    {
        // Construct chain of responsibility
        std::shared_ptr<Math::RegressionModelAlgorithmOLSChain> head =
                std::make_shared<Math::RegressionModelAlgorithmOLSChain>(Math::RegressionModelAlgorithmOLSChain());

        std::shared_ptr<Math::RegressionModelAlgorithmOLSChain> firstLink =
                std::make_shared<Math::RegressionModelOLSLinkCholesky>(Math::RegressionModelOLSLinkCholesky());

        std::shared_ptr<Math::RegressionModelAlgorithmOLSChain> lastLink =
                std::make_shared<Math::RegressionModelOLSLinkMoorePenrose>(Math::RegressionModelOLSLinkMoorePenrose());

        // Add chain links to head in order of priority execution
        head -> addLink(firstLink), head -> addLink(lastLink);

        // Recursively descend chain and return pointer to algorithm being used for calibration
        m_algorithmPtr = head -> handle(coefficients, dependentVariableValues, independentVariableValues);
    }
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


//
// Implementation of recursive descent chain of responsibility for OLS linear system solution
//
Math::RegressionModelAlgorithmOLSChain::RegressionModelAlgorithmOLSChain() : m_nextLink(nullptr)
{

}

void Math::RegressionModelAlgorithmOLSChain::addLink(const std::shared_ptr<Math::RegressionModelAlgorithmOLSChain> &link)
{
    if (m_nextLink)
        m_nextLink -> addLink(link);
    else
        m_nextLink = link;
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmOLSChain::handle(boost::numeric::ublas::vector<double> &coefficients,
                                                                                               const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                                                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    if (m_nextLink)
        return m_nextLink -> handle(coefficients, dependentVariableValues, independentVariableValues);
    else
        throw std::runtime_error("Math::RegressionModelAlgorithmOLSChain::calibrate : impossible to run OLS algorithm chain.");
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelOLSLinkCholesky::handle(boost::numeric::ublas::vector<double> &coefficients,
                                                                                             const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                                                             const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    Math::RegressionModelAlgorithmCholesky ch;
    ch.calibrate(coefficients, dependentVariableValues, independentVariableValues);

    if (ch.hasFailed())
        return Math::RegressionModelAlgorithmOLSChain::handle(coefficients, dependentVariableValues, independentVariableValues);
    else
        return std::make_unique<Math::RegressionModelAlgorithmCholesky>(ch);
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelOLSLinkMoorePenrose::handle(boost::numeric::ublas::vector<double> &coefficients,
                                                                                                 const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                                                                 const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    Math::RegressionModelAlgorithmMoorePenrose mp;
    mp.calibrate(coefficients, dependentVariableValues, independentVariableValues);

    if (mp.hasFailed())
        return Math::RegressionModelAlgorithmOLSChain::handle(coefficients, dependentVariableValues, independentVariableValues);
    else
        return std::make_unique<Math::RegressionModelAlgorithmMoorePenrose>(mp);
}


//
// Regression model algorithm interface implementation
//
Math::ANOVASummary Math::RegressionModelAlgorithm::getANOVA() const
{
    Math::ANOVA anova;
    return anova.computeANOVA(m_coefficients, m_depVariableVals, m_indepVariableVals, *this);
}

Math::RegressionModelAlgorithmMoorePenrose::RegressionModelAlgorithmMoorePenrose() : m_invXtX(), m_isInvertible(true)
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
        m_isInvertible = false;
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
    if (!m_isInvertible)
        return;

    const boost::numeric::ublas::vector<double> XtY = boost::numeric::ublas::prod(Xt, dependentVariableValues);
    coefficients = boost::numeric::ublas::prod(m_invXtX, XtY);
    m_coefficients = coefficients;
}

bool Math::RegressionModelAlgorithmMoorePenrose::hasFailed() const
{
    return !m_isInvertible;
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

    // solve lower triangular system Lt*w=y for x by forward substitution
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

    // solve lower triangular system Lt*W=Y for x by forward substitution
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

    // solve upper triangular system L*X=W for x by backward substitution
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
    const boost::numeric::ublas::matrix<double> sigmaSquaredI =
            residualVariance * boost::numeric::ublas::identity_matrix<double>(m_coefficients.size());
    return _choleskySolve(m_ch.getCholeskyFactor(), sigmaSquaredI);
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmCholesky::clone() const
{
    return std::make_unique<RegressionModelAlgorithmCholesky> (*this);
}