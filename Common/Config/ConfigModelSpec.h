//
// Created by Alberto Campi on 27/07/2019.
//

#ifndef WILDCATSTKCORE_CONFIGMODELSPEC_H
#define WILDCATSTKCORE_CONFIGMODELSPEC_H

#include <memory>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
//#include "../Concepts.h"
#include "ConfigVariable.h"


namespace Math
{
    class RelativeModel;
}

namespace Common
{
    class DataSet;

    class ConfigModelSpec
    {
    public:
        ConfigModelSpec(const Common::ConfigVariable &dependentVariable,
                        std::vector<Common::ConfigVariable> independentVariables);

        Common::ConfigVariable getDependentVariable() const;

        std::vector<Common::ConfigVariable> getIndependentVariables() const;

        virtual void calibrate(const Common::DataSet &ds) = 0;

        virtual double predict(const Common::DataSet &ds, unsigned int index) const = 0;

    protected:
        const Common::ConfigVariable m_dVariable;
        const std::vector<Common::ConfigVariable> m_idVariables;
        std::vector<double> m_coeff;
    };


    class ConfigModelSpecRelative : public ConfigModelSpec//, private Uncopyable
    {
    public:
        ConfigModelSpecRelative(const Common::ConfigVariable &dependentVariable,
                                const std::vector<Common::ConfigVariable> &independentVariables,
                                const std::string &modelSubType,
                                int multiplier);

        void calibrate(const Common::DataSet &ds) final;

        double predict(const Common::DataSet &ds, unsigned int index) const final;

    private:
        std::unique_ptr<Math::RelativeModel> m_modelPtr;
        const int m_multiplier;

    };


    class ConfigModelSpecRegression : public ConfigModelSpec//, private Uncopyable
    {
    public:
        ConfigModelSpecRegression(const Common::ConfigVariable &dependentVariable,
                                  const std::vector<Common::ConfigVariable> &independentVariables,
                                  const std::string &modelSubType,
                                  const boost::gregorian::date &regressionStartDate);

        void calibrate(const Common::DataSet &ds) final;

        double predict(const Common::DataSet &ds, unsigned int index) const final;

        boost::gregorian::date getFirstValidRegressionDate(const Common::DataSet &ds) const;

    private:
        //std::unique_ptr<Math::RegressionModel> m_modelPtr;
        boost::gregorian::date m_startDate;
    };

}
#endif //WILDCATSTKCORE_CONFIGMODELSPEC_H
