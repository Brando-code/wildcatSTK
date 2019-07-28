//
// Created by Alberto Campi on 28/07/2019.
//

#ifndef WILDCATSTKCORE_RELATIVEMODEL_H
#define WILDCATSTKCORE_RELATIVEMODEL_H

#include "../Math.h"
#include "../../Types/TimeSeries.h"
#include <vector>

class Math::RelativeModel
{
public:
    virtual void calibrate(std::vector<double>& coefficients,
                           const std::vector<double>& dependentVariableValues,
                           const std::vector<double>& independentVariableValues) const = 0;

    virtual ~RelativeModel() = default;
};


class Math::RelativeGrowthModel : public Math::RelativeModel
{
public:
    void calibrate(std::vector<double>& coefficients,
                   const std::vector<double>& dependentVariableValues,
                   const std::vector<double>& independentVariableValues) const final;
};


class Math::RelativeModelFactory
{
public:
    virtual std::unique_ptr<Math::RelativeModel> create() const = 0;

    virtual ~RelativeModelFactory() = default;
};

class Math::RelativeGrowthModelFactory : public Math::RelativeModelFactory
{
public:
    std::unique_ptr<Math::RelativeModel> create() const final;
};

#endif //WILDCATSTKCORE_RELATIVEMODEL_H
