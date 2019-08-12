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

void Math::RegressionModelOLS::calibrate(std::vector<double> &coefficients, const std::vector<double> &dependentVariableValues,
                                         const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    //puntatore -> calibrate(args);

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