//
// Created by Alberto Campi on 2019-08-03.
//


#include "RegressionModel.h"
#include "../Statistics/Stat.h"
#include <boost/numeric/ublas/lu.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/mat_traits_array.hpp>

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
Math::ANOVA Math::RegressionModel::getANOVA() const
{
    return m_anova;
}

void Math::RegressionModel::_ANOVATotal()
{
    Math::UnivariateStat acc;
    for (const auto& value : m_depVariableVals)
        acc.add(value);

    m_anova.totalMean = acc.mean();

    m_anova.sampleSize = m_depVariableVals.size();
    m_anova.totalDoF = m_anova.sampleSize - 1;
    m_anova.totalMSEVariance = acc.variance() * m_depVariableVals.size() / m_anova.totalDoF;
}

void Math::RegressionModel::_ANOVAResiduals()
{
    m_anova.fittedValues = boost::numeric::ublas::prod(m_indepVariableVals, m_coefficients);
    m_anova.residuals = m_depVariableVals - m_anova.fittedValues;

    m_anova.residualDoF = m_anova.sampleSize - m_indepVariableVals.size2();
    m_anova.residualMSEVariance = boost::numeric::ublas::inner_prod(m_anova.residuals, m_anova.residuals) / m_anova.residualDoF;
}

void Math::RegressionModel::_ANOVAModel()
{
    const boost::numeric::ublas::vector<double> modelError = m_anova.fittedValues -
                                                boost::numeric::ublas::vector<double>(m_anova.fittedValues.size(), m_anova.totalMean);

    m_anova.modelDoF = m_indepVariableVals.size2() - 1;
    m_anova.modelMSEVariance = boost::numeric::ublas::inner_prod(modelError, modelError) / m_anova.modelDoF;
}

void Math::RegressionModel::_ANOVARSquared()
{
    m_anova.adjRSquared = 1 - m_anova.residualMSEVariance / m_anova.totalMSEVariance;
    m_anova.RSquared = 1 - (m_anova.residualMSEVariance * m_anova.residualDoF) / (m_anova.totalMSEVariance * m_anova.totalDoF);
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
    {
        m_algorithmPtr -> calibrate(coefficients, dependentVariableValues, independentVariableValues);
        m_coefficients = coefficients, m_depVariableVals = dependentVariableValues, m_indepVariableVals = independentVariableValues;
    }
    else
        throw std::runtime_error("Math::RegressionModelOLS::calibrate : "
                                 "at least two observations are needed for regression model to run.");
}

void Math::RegressionModelOLS::computeANOVA()
{
    //Compute ANOVA for dependent variable (total)
    _ANOVATotal();

    //Compute ANOVA for residuals
    _ANOVAResiduals();

    //Compute ANOVA for predictor (fitted model)
    _ANOVAModel();

    //Compute R-squared measures
    _ANOVARSquared();
}

std::unique_ptr<Math::RegressionModel> Math::RegressionModelOLS::clone() const
{
    return std::make_unique<Math::RegressionModelOLS>(*this);
}
//Regression by Moore-Penrose method

void Math::RegressionModelAlgorithmMoorePenrose::calibrate(boost::numeric::ublas::vector<double> &coefficients,
                                                           const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                           const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
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
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmMoorePenrose::clone() const
{
    return std::make_unique<RegressionModelAlgorithmMoorePenrose> (*this);
}

void Math::RegressionModelAlgorithmCholesky::calibrate(boost::numeric::ublas::vector<double> &coefficients,
                                                       const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
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
            sum += choleskyFactor(i,j) * omegaVector(j);
        }
        omegaVector(i) = (productAb(i) - sum) / choleskyFactor(i,i);
    }

    // solve upper triangular system L*x=w for x by backward substitution
    const boost::numeric::ublas::matrix<double> transpCholeskyFactor(boost::numeric::ublas::trans(choleskyFactor));
    for (int i = dim - 1; i >= 0 ; --i)
    {
        double sum = 0;
        for (unsigned int j = i + 1; j < dim ; ++j)
        {
            sum += transpCholeskyFactor(i,j) * coefficients(j);
        }
        coefficients(i) = (omegaVector(i) - sum) / transpCholeskyFactor(i,i);
    }
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmCholesky::clone() const
{
    return std::make_unique<RegressionModelAlgorithmCholesky> (*this);
}