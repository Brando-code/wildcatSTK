//
// Created by Alberto Campi on 28/07/2019.
//

#ifndef WILDCATSTKCORE_RELATIVEMODEL_H
#define WILDCATSTKCORE_RELATIVEMODEL_H

#include <vector>
#include <memory>

namespace Math
{
    class RelativeModel
    {
    public:
        virtual void calibrate(double &coefficient,
                               const std::vector<double> &dependentVariableValues,
                               const std::vector<double> &independentVariableValues) const = 0;

        virtual std::unique_ptr<Math::RelativeModel> clone() const = 0;
        virtual ~RelativeModel() = default;
    };


    class RelativeGrowthModel : public RelativeModel
    {
    public:
        void calibrate(double &coefficient,
                       const std::vector<double> &dependentVariableValues,
                       const std::vector<double> &independentVariableValues) const final;

        std::unique_ptr<Math::RelativeModel> clone() const final;
    };

    class RelativeVolatilityModel : public RelativeModel
    {
    public:
        void calibrate(double &coefficient,
                       const std::vector<double> &dependentVariableValues,
                       const std::vector<double> &independentVariableValues) const final;

        std::unique_ptr<Math::RelativeModel> clone() const final;
    };


    class RelativeModelFactory
    {
    public:
        virtual std::unique_ptr<Math::RelativeModel> create() const = 0;

        virtual ~RelativeModelFactory() = default;
    };

    class RelativeGrowthModelFactory : public RelativeModelFactory
    {
    public:
        std::unique_ptr<Math::RelativeModel> create() const final;
    };

    class RelativeVolatilityModelFactory : public RelativeModelFactory
    {
    public:
        std::unique_ptr<Math::RelativeModel> create() const final;
    };
}
#endif //WILDCATSTKCORE_RELATIVEMODEL_H
