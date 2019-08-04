//
// Created by Alberto Campi on 2019-08-03.
//

#ifndef WILDCATSTKCORE_REGRESSIONMODEL_H
#define WILDCATSTKCORE_REGRESSIONMODEL_H

#include "../Math.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

class Math::RegressionModel
{
public:
    virtual void calibrate(std::vector<double>& coefficients,
                           const std::vector<double>& dependentVariableValues,
                           const boost::numeric::ublas::matrix<double>& independentVariableValues) const = 0;

    virtual ~RegressionModel() = default;
};

class Math::RegressionModelOLS : public Math::RegressionModel
{
public:
    //RegressionModelOLS(algoritmObject);
    void calibrate(std::vector<double>& coefficients,
                   const std::vector<double>& dependentVariableValues,
                   const boost::numeric::ublas::matrix<double>& independentVariableValues) const final;

private:
    //pointer to algorithm implementation
};

#endif //WILDCATSTKCORE_REGRESSIONMODEL_H
