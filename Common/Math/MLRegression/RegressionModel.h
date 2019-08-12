//
// Created by Alberto Campi on 2019-08-03.
//

#ifndef WILDCATSTKCORE_REGRESSIONMODEL_H
#define WILDCATSTKCORE_REGRESSIONMODEL_H

#include "../Math.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>


namespace Math
{
    class RegressionModelAlgorithm;
    class RegressionModelAlgorithmMoorePenrose;

    boost::numeric::ublas::matrix<double> inverseBST (const boost::numeric::ublas::matrix<double> &inputMatrix);
    boost::numeric::ublas::vector<double> stdVec2bstVec (const std::vector<double> &inputVector);
    std::vector<double> bstVec2stdVec (const boost::numeric::ublas::vector<double> &inputVector);
}

class Math::RegressionModel
{
public:
    virtual void calibrate(std::vector<double>& coefficients,
                           const std::vector<double>& dependentVariableValues,
                           const boost::numeric::ublas::matrix<double>& independentVariableValues) const = 0;

    virtual ~RegressionModel() = default;

    virtual std::unique_ptr<Math::RegressionModel> clone() const = 0;
};

class Math::RegressionModelOLS : public Math::RegressionModel
{
public:
    void calibrate(std::vector<double>& coefficients,
                   const std::vector<double>& dependentVariableValues,
                   const boost::numeric::ublas::matrix<double>& independentVariableValues) const final ;

    //virtual ~RegressionModelOLS() = default;

    std::unique_ptr<Math::RegressionModel> clone() const final;

private:
    //std::unique_ptr<Math::RegressionModelAlgorithm> m_mdlPtr;
    //std::unique_ptr<Math::RegressionModelAlgorithmMoorePenrose> m_mdPtr;

};

class Math::RegressionModelAlgorithm
{
public:
    virtual void calibrate(std::vector<double>& coefficients,
                           const std::vector<double>& dependentVariableValues,
                           const boost::numeric::ublas::matrix<double>& independentVariableValues) const = 0;

    virtual ~RegressionModelAlgorithm() = default;

    virtual std::unique_ptr<Math::RegressionModelAlgorithm> clone() const = 0;
};


class Math::RegressionModelAlgorithmMoorePenrose : public Math::RegressionModelAlgorithm
{
public:
    void calibrate(std::vector<double>& coefficients,
                   const std::vector<double>& dependentVariableValues,
                   const boost::numeric::ublas::matrix<double>& independentVariableValues) const final;

    std::unique_ptr<Math::RegressionModelAlgorithm> clone() const final;
};

#endif //WILDCATSTKCORE_REGRESSIONMODEL_H
