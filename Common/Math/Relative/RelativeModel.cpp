//
// Created by Alberto Campi on 28/07/2019.
//

#include "RelativeModel.h"
#include "../Statistics/Stat.h"


//RelativeGrowthModel class implementation
void Math::RelativeGrowthModel::calibrate(double &coefficient, const std::vector<double>& dependentVariableValues,
                                          const std::vector<double>& independentVariableValues) const
{
    coefficient = 1.0;
}

std::unique_ptr<Math::RelativeModel> Math::RelativeGrowthModel::clone() const
{
    return std::make_unique<Math::RelativeGrowthModel>(*this);
}


//RelativeVolatilityModel class implementation
void Math::RelativeVolatilityModel::calibrate(double &coefficient, const std::vector<double> &dependentVariableValues,
                                              const std::vector<double> &independentVariableValues) const
{
    Math::UnivariateStat dVar, idVar;
    for (const auto& val: dependentVariableValues)
        dVar.add(val);

    for (const auto& val: independentVariableValues)
        idVar.add(val);

    coefficient = idVar.stdDev() / dVar.stdDev();
}

std::unique_ptr<Math::RelativeModel> Math::RelativeVolatilityModel::clone() const
{
    return std::make_unique<Math::RelativeVolatilityModel>(*this);
}

//Abstract factory classes implementation
std::unique_ptr<Math::RelativeModel> Math::RelativeGrowthModelFactory::create() const
{
    return std::make_unique<Math::RelativeGrowthModel>(RelativeGrowthModel());
}

std::unique_ptr<Math::RelativeModel> Math::RelativeVolatilityModelFactory::create() const
{
    return std::make_unique<Math::RelativeVolatilityModel>(RelativeVolatilityModel());
}