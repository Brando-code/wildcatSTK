//
// Created by Alberto Campi on 27/07/2019.
//

#ifndef WILDCATSTKCORE_CONFIGMODELSPEC_H
#define WILDCATSTKCORE_CONFIGMODELSPEC_H

#include <memory>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "ConfigVariable.h"


namespace Math
{
    class RelativeModel;
    class RegressionModel;
}

namespace Common
{
    class DataSet;

    class ConfigModelSpec
    {
    public:
        ConfigModelSpec(const Common::ConfigVariable &dependentVariable,
                        std::vector<Common::ConfigVariable> independentVariables);
        ConfigModelSpec(const ConfigModelSpec& other);
        ConfigModelSpec(ConfigModelSpec&& other);

        Common::ConfigVariable getDependentVariable() const;

        std::vector<Common::ConfigVariable> getIndependentVariables() const;

        virtual void calibrate(const Common::DataSet &ds) = 0;
        virtual double predict(const Common::DataSet &ds, unsigned int index) const = 0;

        virtual std::unique_ptr<Common::ConfigModelSpec> clone() const = 0;

    protected:
        Common::ConfigVariable m_dVariable;
        std::vector<Common::ConfigVariable> m_idVariables;
        std::vector<double> m_coeff;
    };


    class ConfigModelSpecRelative : public ConfigModelSpec
    {
    public:
        ConfigModelSpecRelative(const Common::ConfigVariable &dependentVariable,
                                const std::vector<Common::ConfigVariable> &independentVariables,
                                const std::string &modelSubType,
                                int multiplier);
        ConfigModelSpecRelative(const Common::ConfigModelSpecRelative& other);
        ConfigModelSpecRelative& operator=(const Common::ConfigModelSpecRelative& other);
        ConfigModelSpecRelative(Common::ConfigModelSpecRelative&& other);
        ConfigModelSpecRelative& operator=(Common::ConfigModelSpecRelative&& other);

        void calibrate(const Common::DataSet &ds) final;
        double predict(const Common::DataSet &ds, unsigned int index) const final;

        std::unique_ptr<Common::ConfigModelSpec> clone() const final;

    private:
        std::unique_ptr<Math::RelativeModel> m_modelPtr;
        int m_multiplier;

    };


    class ConfigModelSpecRegression : public ConfigModelSpec
    {
    public:
        ConfigModelSpecRegression(const Common::ConfigVariable &dependentVariable,
                                  const std::vector<Common::ConfigVariable> &independentVariables,
                                  const std::string &modelSubType,
                                  const boost::gregorian::date &regressionStartDate);
        ConfigModelSpecRegression(const Common::ConfigModelSpecRegression& other);
        ConfigModelSpecRegression& operator=(const Common::ConfigModelSpecRegression& other);
        ConfigModelSpecRegression(Common::ConfigModelSpecRegression&& other);
        ConfigModelSpecRegression& operator=(Common::ConfigModelSpecRegression&& other);

        void calibrate(const Common::DataSet &ds) final;
        double predict(const Common::DataSet &ds, unsigned int index) const final;

        boost::gregorian::date getFirstValidRegressionDate(const Common::DataSet &ds) const;

        std::unique_ptr<Common::ConfigModelSpec> clone() const final;

    private:
        std::unique_ptr<Math::RegressionModel> m_modelPtr;
        boost::gregorian::date m_startDate;
    };

}
#endif //WILDCATSTKCORE_CONFIGMODELSPEC_H
