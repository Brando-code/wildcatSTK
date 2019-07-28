//
// Created by Alberto Campi on 28/07/2019.
//

#include "RelativeModel.h"

void Math::RelativeGrowthModel::calibrate(std::vector<double>& coefficients,
                                          const std::vector<double>& dependentVariableValues,
                                          const std::vector<double>& independentVariableValues) const
{
    coefficients[0] = 1.0;
}

std::unique_ptr<Math::RelativeModel> Math::RelativeGrowthModelFactory::create() const
{
    return std::make_unique<Math::RelativeGrowthModel>(RelativeGrowthModel());
}