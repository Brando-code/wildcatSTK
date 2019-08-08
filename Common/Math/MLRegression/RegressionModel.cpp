//
// Created by Alberto Campi on 2019-08-03.
//

#include "RegressionModel.h"

void Math::RegressionModelOLS::calibrate(std::vector<double> &coefficients, const std::vector<double> &dependentVariableValues,
                                         const boost::numeric::ublas::matrix<double> &independentVariableValues) const
{
    //delegate task to algorithm pointer
}

std::unique_ptr<Math::RegressionModel> Math::RegressionModelOLS::clone() const
{
    return std::make_unique<Math::RegressionModelOLS>(*this);
}