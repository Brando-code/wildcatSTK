//
// Created by Alberto Campi on 27/07/2019.
//

#ifndef WILDCATSTKCORE_CONFIGMODELSPEC_H
#define WILDCATSTKCORE_CONFIGMODELSPEC_H

#include <memory>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include "ConfigVariable.h"
#include "../Math/MLRegression/RegressionModel.h"


namespace Math
{
    class RelativeModel;
    class RegressionModel;
}

namespace Common
{
    class DataSet;

    //
    // ConfigModelSpec base class, abstraction for model-config-type model specification (enhanced equation)
    //
    class ConfigModelSpec
    {
    public:
        ConfigModelSpec(const Common::ConfigVariable &dependentVariable,
                        std::vector<Common::ConfigVariable> independentVariables);

        Common::ConfigVariable getDependentVariable() const;
        std::vector<Common::ConfigVariable> getIndependentVariables() const;
        unsigned int getMaxLag() const;

        virtual void calibrate(const Common::DataSet &ds) = 0;
        virtual double predict(const Common::DataSet &ds, const boost::gregorian::date& date) const = 0;

        virtual std::unique_ptr<Common::ConfigModelSpec> clone() const = 0;

        virtual bool operator==(const Common::ConfigModelSpec& other) const = 0;
        virtual bool operator!=(const Common::ConfigModelSpec& other) const = 0;

    protected:
        Common::ConfigVariable m_dVariable;
        std::vector<Common::ConfigVariable> m_idVariables;

        bool _equal(const Common::ConfigModelSpec& other) const;
    };


    //
    // Specialization for relative model sub-type specifications
    //
    class ConfigModelSpecRelative : public ConfigModelSpec
    {
    public:
        ConfigModelSpecRelative(const Common::ConfigVariable &dependentVariable,
                                const std::vector<Common::ConfigVariable> &independentVariables,
                                const std::string &modelSubType,
                                double multiplier);
        ConfigModelSpecRelative(const Common::ConfigModelSpecRelative& other);
        ConfigModelSpecRelative& operator=(const Common::ConfigModelSpecRelative& other);
        ConfigModelSpecRelative(Common::ConfigModelSpecRelative&& other);
        ConfigModelSpecRelative& operator=(Common::ConfigModelSpecRelative&& other);

        double getMultiplier() const;
        std::string getModelSubType() const;

        void calibrate(const Common::DataSet &ds) final;
        double predict(const Common::DataSet &ds, const boost::gregorian::date& date) const final;

        std::unique_ptr<Common::ConfigModelSpec> clone() const final;

        bool operator==(const Common::ConfigModelSpec& other) const final;
        bool operator!=(const Common::ConfigModelSpec& other) const final;

    private:
        std::unique_ptr<Math::RelativeModel> m_modelPtr;
        double m_multiplier;
        std::string m_modelSubType;
        double m_coeff;
    };


    //
    // Specialization for regression model sub-type specifications
    //
    class ConfigModelSpecRegression : public ConfigModelSpec
    {
    public:
        ConfigModelSpecRegression(const Common::ConfigVariable &dependentVariable,
                                  const std::vector<Common::ConfigVariable> &independentVariables,
                                  const std::string &modelSubType,
                                  const boost::gregorian::date &regressionStartDate,
                                  bool computeAnova = true);
        ConfigModelSpecRegression(const Common::ConfigModelSpecRegression& other);
        ConfigModelSpecRegression& operator=(const Common::ConfigModelSpecRegression& other);
        ConfigModelSpecRegression(Common::ConfigModelSpecRegression&& other);
        ConfigModelSpecRegression& operator=(Common::ConfigModelSpecRegression&& other);

        std::string getModelSubType() const;

        void calibrate(const Common::DataSet &ds) final;
        double predict(const Common::DataSet &ds, const boost::gregorian::date& date) const final;
        Math::ANOVASummary getANOVASummary() const;

        boost::gregorian::date getFirstValidRegressionDate(const Common::DataSet &ds) const;
        std::vector<double> getCalibratedCoefficients() const;

        std::unique_ptr<Common::ConfigModelSpec> clone() const final;

        bool operator==(const Common::ConfigModelSpec& other) const final;
        bool operator!=(const Common::ConfigModelSpec& other) const final;

    private:
        std::unique_ptr<Math::RegressionModel> m_modelPtr;
        boost::gregorian::date m_startDate;
        std::string m_modelSubType;
        std::vector<double> m_params;
        bool m_computeAnovaFlag;

        boost::numeric::ublas::vector<double> _getTransformedValues(const Common::TimeSeries& ts,
                                                                    const boost::gregorian::date& firstDate,
                                                                    const Common::ConfigVariable& variable) const;
    };

}
#endif //WILDCATSTKCORE_CONFIGMODELSPEC_H
