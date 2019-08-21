//
// Created by Alberto Campi on 2019-08-03.
//

/* AUXILIARY FUNCTIONS
 *--------------------------------------------------------------------------------------------------------------------*/
#include "RegressionModel.h"
#include <boost/numeric/ublas/lu.hpp>


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

// Cholesky Decomposition
boost::numeric::ublas::matrix<double> Math::choleskyDecomp(const boost::numeric::ublas::matrix<double> &inputMatrix)
{
    int dim = inputMatrix.size1();

    boost::numeric::ublas::matrix<double> outputMatrix(boost::numeric::ublas::zero_matrix<double>(dim,dim));

    double sum;
    for (int i = 0; i < dim ; ++i)
    {
        for (int k = 0; k < i + 1; ++k)
        {
            sum = 0;
            for (int j = 0; j < k; ++j)
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
    boost::numeric::ublas::vector<double> outputVector;
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
//Math::RegressionModelOLS::RegressionModelOLS(const Math::RegressionModelAlgorithm &regressionAlgorithm) :
//    m_algorithmPtr(regressionAlgorithm.clone()) {}

void Math::RegressionModelOLS::calibrate(std::vector<double> &coefficients, const std::vector<double> &dependentVariableValues,
                                         const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    //m_algorithmPtr ->calibrate(coefficients, dependentVariableValues, independentVariableValues);
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

    boost::numeric::ublas::matrix<double> transposedIndiVal(boost::numeric::ublas::trans(independentVariableValues));

    boost::numeric::ublas::matrix<double> productTransIndiVal(prod(transposedIndiVal, independentVariableValues));

    boost::numeric::ublas::matrix<double> inverseProduct(inverseBST(productTransIndiVal));

    boost::numeric::ublas::matrix<double> productInverseTransposed(prod(inverseProduct, transposedIndiVal));

    boost::numeric::ublas::vector<double> dependentVariable(Math::stdVec2bstVec(dependentVariableValues));

    boost::numeric::ublas::vector<double>  temp_coefficients(prod(productInverseTransposed, dependentVariable));
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
    boost::numeric::ublas::matrix<double>choleskyFactor(choleskyDecomp(independentVariableValues));

    int dim = choleskyFactor.size1();

    // solve lower triangular system L*w=A*b for w by forward substitution (* = transposition)
    boost::numeric::ublas::matrix<double>transpCholeskyFactor(trans(choleskyFactor));

    boost::numeric::ublas::matrix<double> transposedIndiVal(trans(independentVariableValues));

    boost::numeric::ublas::vector<double> dependentVariable(Math::stdVec2bstVec(dependentVariableValues));

    boost::numeric::ublas::vector<double> productAb(prod(transposedIndiVal, dependentVariable));

    boost::numeric::ublas::vector<double> omegaVector;
    omegaVector(0) = (productAb(0) / choleskyFactor(0,0));

    double sum;
    for (int i = 1; i < dim; ++i)
    {
        sum = 0;
        for (int j = 0; j < i; ++j)
        {
            sum += transpCholeskyFactor(i,j) * omegaVector(j);
        }

        omegaVector(i) = (productAb(i) - sum) / transpCholeskyFactor(i,i);
    }

    // solve upper triangular system Lx=w for x by backward substitution
    boost::numeric::ublas::vector<double> xVector(dim);

    xVector(dim) = omegaVector(dim) / choleskyFactor(dim,dim);

    for (int i = dim - 1; i >= 0 ; --i)
    {
        sum = 0;
        for (int j = i + 1; j <= dim ; ++j)
        {
            sum += choleskyFactor(i,j) * xVector(j);
        }
        xVector(i) = (omegaVector(i) - sum) / choleskyFactor(i,i);
    }

    coefficients = Math::bstVec2stdVec(xVector);
}

std::unique_ptr<Math::RegressionModelAlgorithm> Math::RegressionModelAlgorithmCholesky::clone() const
{
    return std::make_unique<RegressionModelAlgorithmCholesky> (*this);
}