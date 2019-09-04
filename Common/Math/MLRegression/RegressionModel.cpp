//
// Created by Alberto Campi on 2019-08-03.
//

/* AUXILIARY FUNCTIONS
 *--------------------------------------------------------------------------------------------------------------------*/
#include "RegressionModel.h"
#include <boost/numeric/ublas/lu.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/mat_traits_array.hpp>


// Matrix inversion
boost::numeric::ublas::matrix<double> Math::inverseBST (const boost::numeric::ublas::matrix<double> &inputMatrix)
{
    boost::numeric::ublas::matrix<double> tempInputMatrix(inputMatrix);

    boost::numeric::ublas::matrix<double> invertedMatrix = boost::numeric::ublas::identity_matrix<double>(inputMatrix.size1());
    boost::numeric::ublas::permutation_matrix<size_t> permutationMatrix(inputMatrix.size1());
    lu_factorize(tempInputMatrix, permutationMatrix);
    lu_substitute(tempInputMatrix, permutationMatrix, invertedMatrix);

    return invertedMatrix;
}

// Matrix determinant sign
int Math::determinantSign(const boost::numeric::ublas::permutation_matrix<std::size_t> &permutationMatrix)
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
double Math::matrixDeterminant(boost::numeric::ublas::matrix<double> inputMatrix)
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
        determinant *= determinantSign(permutationMatrix);
    }
    return determinant;
}

// Cholesky Decomposition
boost::numeric::ublas::matrix<double> Math::choleskyDecomp(const boost::numeric::ublas::matrix<double> &inputMatrix)
{
    int dim = inputMatrix.size1();

    boost::numeric::ublas::matrix<double> outputMatrix(boost::numeric::ublas::zero_matrix<double>(dim,dim));

    double sum;
    for (unsigned int i = 0; i < dim ; ++i)
    {
        for (unsigned int k = 0; k < i + 1; ++k)
        {
            sum = 0;
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

// From std vector to Boost vector conversation
boost::numeric::ublas::vector<double> Math::stdVec2bstVec (const std::vector<double> &inputVector)
{
    boost::numeric::ublas::vector<double> outputVector(inputVector.size());
    for (unsigned long i = 0; i < outputVector.size(); ++i)
        outputVector(i) = inputVector.at(i);

    return outputVector;
}

// From Boost vector to std vector conversation
std::vector<double> Math::bstVec2stdVec (const boost::numeric::ublas::vector<double> &inputVector)
{
    std::vector<double> outputVector;
    for (unsigned long i = 0; i < inputVector.size(); ++i)
        outputVector.push_back(inputVector(i));

    return outputVector;
}

/*REGRESSION MODEL METHODS
 * --------------------------------------------------------------------------------------------------------*/
Math::RegressionModelOLS::RegressionModelOLS() :
    m_algorithmPtr(Math::RegressionModelAlgorithmCholesky().clone())
{

}

Math::RegressionModelOLS::RegressionModelOLS(const Math::RegressionModelOLS &other) :
    m_algorithmPtr(other.m_algorithmPtr -> clone())
{

}

void Math::RegressionModelOLS::calibrate(std::vector<double> &coefficients, const std::vector<double> &dependentVariableValues,
                                         const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    if (dependentVariableValues.size() == independentVariableValues.size1() and independentVariableValues.size1() > 2)
        m_algorithmPtr -> calibrate(coefficients, dependentVariableValues, independentVariableValues);
    else
        throw std::runtime_error("Math::RegressionModelOLS::calibrate : "
                                 "at least two observations are needed for regression model to run.");
}

std::unique_ptr<Math::RegressionModel> Math::RegressionModelOLS::clone() const
{
    return std::make_unique<Math::RegressionModelOLS>(*this);
}
//Regression by Moore-Penrose method

void Math::RegressionModelAlgorithmMoorePenrose::calibrate(std::vector<double> &coefficients,
                                               const std::vector<double> &dependentVariableValues,
                                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    coefficients.clear();

    const boost::numeric::ublas::matrix<double> transposedIndiVal(boost::numeric::ublas::trans(independentVariableValues));
    const boost::numeric::ublas::matrix<double> productTransIndiVal(prod(transposedIndiVal, independentVariableValues));

    // Invertible controller
    double determinantMatrix(Math::matrixDeterminant(productTransIndiVal));
    if (determinantMatrix == 0.)
        throw std::runtime_error("Error: matrix is not invertible.");

    const boost::numeric::ublas::matrix<double> inverseProduct(inverseBST(productTransIndiVal));
    const boost::numeric::ublas::matrix<double> productInverseTransposed(prod(inverseProduct, transposedIndiVal));
    const boost::numeric::ublas::vector<double> dependentVariable = Math::stdVec2bstVec(dependentVariableValues);

    const boost::numeric::ublas::vector<double>  temp_coefficients(prod(productInverseTransposed, dependentVariable));
    coefficients = Math::bstVec2stdVec(temp_coefficients);
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmMoorePenrose::clone() const
{
    return std::make_unique<RegressionModelAlgorithmMoorePenrose> (*this);
}

void Math::RegressionModelAlgorithmCholesky::calibrate(std::vector<double> &coefficients,
                                                       const std::vector<double> &dependentVariableValues,
                                                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    const boost::numeric::ublas::matrix<double> transposedIndiVal(boost::numeric::ublas::trans(independentVariableValues));
    const boost::numeric::ublas::matrix<double> productTransIndiVal = prod(transposedIndiVal, independentVariableValues);
    const boost::numeric::ublas::matrix<double> choleskyFactor(choleskyDecomp(productTransIndiVal));

    const int dim = choleskyFactor.size1();

    // solve lower triangular system Lw=A*b for w by forward substitution (* = transposition)

    const boost::numeric::ublas::vector<double> dependentVariable(Math::stdVec2bstVec(dependentVariableValues));
    const boost::numeric::ublas::vector<double> productAb = prod(transposedIndiVal, dependentVariable);

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
    const boost::numeric::ublas::matrix<double>transpCholeskyFactor(trans(choleskyFactor));
    boost::numeric::ublas::vector<double> xVector(dim);
    for (int i = dim - 1; i >= 0 ; --i)
    {
        double sum = 0;
        for (unsigned int j = i + 1; j < dim ; ++j)
        {
            sum += transpCholeskyFactor(i,j) * xVector(j);
        }
        xVector(i) = (omegaVector(i) - sum) / transpCholeskyFactor(i,i);
    }

    coefficients = Math::bstVec2stdVec(xVector);
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmCholesky::clone() const
{
    return std::make_unique<RegressionModelAlgorithmCholesky> (*this);
}