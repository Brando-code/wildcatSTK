//
// Created by Alberto Campi on 27/07/2019.
//

#include "ConfigModelSpec.h"

#include <utility>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include "../Types/DataSet.h"
#include "../Types/TimeSeries.h"
#include "../../Global/Mappings/FactoryMappings.h"
#include "../Math/MLRegression/RegressionModel.h"
#include "../Math/Relative/RelativeModel.h"


//
// ConfigModelSpec base class methods implementation
//
Common::ConfigModelSpec::ConfigModelSpec(const Common::ConfigVariable &dependentVariable,
                        std::vector<Common::ConfigVariable> independentVariables) :
        m_dVariable(dependentVariable),
        m_idVariables(std::move(independentVariables))
{
    if (m_idVariables.empty())
        throw std::runtime_error("E: ConfigModelSpec::ConfigModelSpec : no drivers specified for dependent variable " +
        m_dVariable.getBasename());
}

Common::ConfigVariable Common::ConfigModelSpec::getDependentVariable() const
{
    return m_dVariable;
}

std::vector<Common::ConfigVariable> Common::ConfigModelSpec::getIndependentVariables() const
{
    return m_idVariables;
}

unsigned int Common::ConfigModelSpec::getMaxLag() const
{
    unsigned int maxLag = m_dVariable.getLagDependency();
    for (const auto& configVariable: m_idVariables)
        if (configVariable.getLagDependency() > maxLag)
            maxLag = configVariable.getLagDependency();

    return maxLag;
}

bool Common::ConfigModelSpec::_equal(const Common::ConfigModelSpec &other) const
{
    return (m_dVariable == other.m_dVariable and m_idVariables == other.m_idVariables);
}


//
//ConfigModelSpecRelative interface implementation
//
Common::ConfigModelSpecRelative::ConfigModelSpecRelative(const Common::ConfigVariable &dependentVariable,
                                                         const std::vector<Common::ConfigVariable> &independentVariables,
                                                         const std::string &modelSubType, double multiplier) :
        ConfigModelSpec(dependentVariable, independentVariables),
        m_multiplier(multiplier),
        m_modelSubType(modelSubType),
        m_coeff(0)
{
    //initialize m_modelPtr via factory
    m_modelPtr = Global::RelativeModelFactoryMapping::instance() -> getFactory(modelSubType) -> create();
}

Common::ConfigModelSpecRelative::ConfigModelSpecRelative(const Common::ConfigModelSpecRelative &other) :
        ConfigModelSpec(other.m_dVariable, other.m_idVariables),
        m_multiplier(other.m_multiplier),
        m_modelSubType(other.m_modelSubType),
        m_modelPtr(other.m_modelPtr -> clone()) {}

Common::ConfigModelSpecRelative::ConfigModelSpecRelative(Common::ConfigModelSpecRelative &&other) :
        ConfigModelSpec(other.m_dVariable, other.m_idVariables),
        m_multiplier(other.m_multiplier),
        m_modelSubType(other.m_modelSubType),
        m_modelPtr(std::move(other.m_modelPtr)) {}

Common::ConfigModelSpecRelative& Common::ConfigModelSpecRelative::operator=(const Common::ConfigModelSpecRelative &other)
{
    if (&other != this)
    {
        m_dVariable = other.m_dVariable;
        m_idVariables = other.m_idVariables;
        m_coeff = other.m_coeff;
        m_multiplier = other.m_multiplier;
        m_modelSubType = other.m_modelSubType;
        m_modelPtr = other.m_modelPtr -> clone();
    }
    return *this;
}

Common::ConfigModelSpecRelative& Common::ConfigModelSpecRelative::operator=(Common::ConfigModelSpecRelative &&other)
{
    if (&other != this)
    {
        m_dVariable = other.m_dVariable;
        m_idVariables = other.m_idVariables;
        m_coeff = other.m_coeff;
        m_multiplier = other.m_multiplier;
        m_modelSubType = other.m_modelSubType;
        m_modelPtr = std::move(other.m_modelPtr);
    }
    return *this;
}

double Common::ConfigModelSpecRelative::getMultiplier() const
{
    return m_multiplier;
}

std::string Common::ConfigModelSpecRelative::getModelSubType() const
{
    return m_modelSubType;
}

void Common::ConfigModelSpecRelative::calibrate(const Common::DataSet &ds)
{
    if (m_idVariables.size() > 1)   //[AC] maybe we should throw?
        std::cerr << "W: ConfigModelSpecRelative::calibrate : variable "
                     << m_dVariable.getBasename() <<  " has multiple drivers for relative model type. Using first driver only..." << std::endl;

    const std::vector<double> transformedDependentVariableValues =
            m_dVariable.getTransformedTimeSeriesValues(ds.getTimeSeries(m_dVariable.getBasename()));
    const std::vector<double> transformedIndependentVariableValues =
            m_idVariables.at(0).getTransformedTimeSeriesValues(ds.getTimeSeries(m_idVariables.at(0).getBasename()));
    
    // Delegate execution to m_modelPtr
    m_modelPtr -> calibrate(m_coeff, transformedDependentVariableValues, transformedIndependentVariableValues);
}

double Common::ConfigModelSpecRelative::predict(const Common::DataSet &ds, const boost::gregorian::date &date) const
{
    if (m_coeff == 0)
        throw std::out_of_range("E: ConfigModelSpecRelative::predict : projections cannot be generated from un-calibrated models.");

    const unsigned long index = ds.getTimeSeries(m_idVariables.at(0).getBasename()).getIndex(date);
    const double scalar = m_multiplier * m_coeff;
    const double transformedProjection =
            scalar * m_idVariables.at(0).getTransformedValue(ds.getTimeSeries(m_idVariables.at(0).getBasename()), index);
    return m_dVariable.getLevel(ds.getTimeSeries(m_dVariable.getBasename()), transformedProjection, index);
    //return predict(ds, index);
}

std::unique_ptr<Common::ConfigModelSpec> Common::ConfigModelSpecRelative::clone() const
{
    return std::make_unique<Common::ConfigModelSpecRelative>(*this);
}

bool Common::ConfigModelSpecRelative::operator==(const Common::ConfigModelSpec &other) const
{
    return _equal(other) and
            m_multiplier == dynamic_cast<const Common::ConfigModelSpecRelative&>(other).m_multiplier and
            m_modelSubType == dynamic_cast<const Common::ConfigModelSpecRelative&>(other).m_modelSubType;
}

bool Common::ConfigModelSpecRelative::operator!=(const Common::ConfigModelSpec &other) const
{
    return !(*this == other);
}


//
//ConfigModelSpecRegression interface implementation
//
Common::ConfigModelSpecRegression::ConfigModelSpecRegression(const Common::ConfigVariable &dependentVariable,
                                                             const std::vector<Common::ConfigVariable> &independentVariables,
                                                             const std::string &modelSubType,
                                                             const boost::gregorian::date &regressionStartDate,
                                                             bool computeAnova):
        ConfigModelSpec(dependentVariable, independentVariables),
        m_startDate(regressionStartDate),
        m_modelSubType(modelSubType),
        m_params(), // [AC] number of id variables + intercept
        m_computeAnovaFlag(computeAnova),
        //Should be replaced by factory when more regression-type models are available
        m_modelPtr(std::make_unique<Math::RegressionModelOLS>(Math::RegressionModelOLS()))
{

}

Common::ConfigModelSpecRegression::ConfigModelSpecRegression(const Common::ConfigModelSpecRegression &other) :
        ConfigModelSpec(other.m_dVariable, other.m_idVariables),
        m_startDate(other.m_startDate),
        m_modelSubType(other.m_modelSubType),
        m_params(other.m_params),
        m_computeAnovaFlag(other.m_computeAnovaFlag),
        m_modelPtr(other.m_modelPtr -> clone())
{

}

Common::ConfigModelSpecRegression::ConfigModelSpecRegression(Common::ConfigModelSpecRegression &&other) :
        ConfigModelSpec(other.m_dVariable, other.m_idVariables),
        m_startDate(other.m_startDate),
        m_modelSubType(other.m_modelSubType),
        m_params(other.m_params),
        m_computeAnovaFlag(other.m_computeAnovaFlag),
        m_modelPtr(std::move(other.m_modelPtr))
{

}

Common::ConfigModelSpecRegression& Common::ConfigModelSpecRegression::operator=(const Common::ConfigModelSpecRegression &other)
{
    if (&other != this)
    {
        m_dVariable = other.m_dVariable;
        m_idVariables = other.m_idVariables;
        m_params = other.m_params;
        m_startDate = other.m_startDate;
        m_modelSubType = other.m_modelSubType;
        m_computeAnovaFlag = other.m_computeAnovaFlag;
        m_modelPtr = other.m_modelPtr -> clone();
    }
    return *this;
}

Common::ConfigModelSpecRegression& Common::ConfigModelSpecRegression::operator=(Common::ConfigModelSpecRegression &&other)
{
    if (&other != this)
    {
        m_dVariable = other.m_dVariable;
        m_idVariables = other.m_idVariables;
        m_params = other.m_params;
        m_startDate = other.m_startDate;
        m_modelSubType = other.m_modelSubType;
        m_computeAnovaFlag = other.m_computeAnovaFlag;
        m_modelPtr = std::move(other.m_modelPtr);
    }
    return *this;
}

std::string Common::ConfigModelSpecRegression::getModelSubType() const
{
    return m_modelSubType;
}

boost::gregorian::date Common::ConfigModelSpecRegression::getFirstValidRegressionDate(const Common::DataSet &ds) const
{
    // Get first valid date across drivers and dependent variable
    boost::gregorian::date firstValidDate = ds.getTimeSeries(m_dVariable.getBasename()).getDates().at(getMaxLag() + 1);
    for (const auto& it: m_idVariables)
    {
        boost::gregorian::date thisDriverFirstAvailableDate = ds.getTimeSeries(it.getBasename()).getDates().at(getMaxLag() + 1);
        if (thisDriverFirstAvailableDate > firstValidDate)
            firstValidDate = thisDriverFirstAvailableDate;
    }

    // Check that the client-specified regression start date is more recent than the first available start date
    const std::vector<boost::gregorian::date> dVariableDates = ds.getTimeSeries(m_dVariable.getBasename()).getDates();
    if (m_startDate > firstValidDate && std::find(dVariableDates.begin(), dVariableDates.end(), m_startDate) != dVariableDates.end())
        firstValidDate = m_startDate;
    else
        std::cerr << "W: ConfigModelSpecRegression::getFirstValidRegressionDate : start regression date for variable " + m_dVariable.getBasename() +
        " is not in the data intersection. Using first valid date..." << std::endl;

    return firstValidDate;
}

boost::numeric::ublas::vector<double> Common::ConfigModelSpecRegression::_getTransformedValues(const Common::TimeSeries &ts,
                                                                                               const boost::gregorian::date &firstDate,
                                                                                               const Common::ConfigVariable &variable) const
{
    const unsigned long firstIndex = ts.getIndex(firstDate);
    boost::numeric::ublas::vector<double> rvs(ts.length() - firstIndex);

    unsigned long j = 0;
    for (unsigned long i = firstIndex; i < ts.length(); ++i)
    {
        rvs(j) = variable.getTransformedValue(ts, i);
        ++j;
    }

    return rvs;
}

void Common::ConfigModelSpecRegression::calibrate(const Common::DataSet &ds)
{
    // Get first available date across drivers and dependent variable
    const boost::gregorian::date firstValidDate = getFirstValidRegressionDate(ds);

    // Construct array of transformed dependent variable values to be used by MLRegression
    const boost::numeric::ublas::vector<double> dVariableValuesForRegression =
            _getTransformedValues(ds.getTimeSeries(m_dVariable.getBasename()), firstValidDate, m_dVariable);

    // Construct matrix of transformed independent variable values to be used by MLRegression
    const unsigned long nRows = dVariableValuesForRegression.size();
    const unsigned long nCols = m_idVariables.size();
    boost::numeric::ublas::matrix<double> idVariableValuesForRegression(nRows, nCols + 1, 1);

    for (unsigned long i = 0; i < nCols; ++i)
    {
        const boost::numeric::ublas::vector<double> idVariableTransformedValues =
                _getTransformedValues(ds.getTimeSeries(m_idVariables.at(i).getBasename()), firstValidDate, m_idVariables.at(i));

        boost::numeric::ublas::column(idVariableValuesForRegression, i) = idVariableTransformedValues;
    }

    // Delegate execution to RegressionModelObject
    boost::numeric::ublas::vector<double> params(idVariableValuesForRegression.size2());
    m_modelPtr -> calibrate(params, dVariableValuesForRegression, idVariableValuesForRegression);

    // Copy coefficients to vector type (std) consumable by downstream code
    m_params.resize(m_idVariables.size() + 1);
    std::copy(params.begin(), params.end(), m_params.begin());
}

double Common::ConfigModelSpecRegression::predict(const Common::DataSet &ds, const boost::gregorian::date &date) const
{
    if (m_params.empty())
        throw std::out_of_range("E: ConfigModelSpecRegression::predict : projections cannot be generated from un-calibrated models.");

    std::vector<unsigned long> idVariablesIndex;
    for (const auto& variable : m_idVariables)
        idVariablesIndex.push_back(ds.getTimeSeries(variable.getBasename()).getIndex(date));

    const double intercept = m_params.back();
    double rhsSum = 0;
    for (unsigned int i = 0; i < m_idVariables.size(); ++i)
    {
        const Common::TimeSeries ts = ds.getTimeSeries(m_idVariables.at(i).getBasename());
        rhsSum += m_params.at(i) * m_idVariables.at(i).getTransformedValue(ts, idVariablesIndex.at(i));
    }

    rhsSum += intercept;

    const Common::TimeSeries ts = ds.getTimeSeries(m_dVariable.getBasename());
    const unsigned long dVariableIndex = ds.getTimeSeries(m_dVariable.getBasename()).length();
    return m_dVariable.getLevel(ts, rhsSum, dVariableIndex);
}

std::vector<double> Common::ConfigModelSpecRegression::getCalibratedCoefficients() const
{
    return m_params;
}

Math::ANOVASummary Common::ConfigModelSpecRegression::getANOVASummary() const
{
    if (!m_computeAnovaFlag)
        std::cerr << "Common::ConfigModelSpecRegression::getANOVASummary : diagnostics are switched off for model " +
                     m_dVariable.getBasename() + ". Empty ANOVA may be returned..." << std::endl;

    return m_modelPtr -> getANOVA();
}

std::unique_ptr<Common::ConfigModelSpec> Common::ConfigModelSpecRegression::clone() const
{
    return std::make_unique<Common::ConfigModelSpecRegression>(*this);
}

bool Common::ConfigModelSpecRegression::operator==(const Common::ConfigModelSpec &other) const
{
    return _equal(other) and
            dynamic_cast<const Common::ConfigModelSpecRegression&>(other).m_startDate == m_startDate and
            dynamic_cast<const Common::ConfigModelSpecRegression&>(other).m_modelSubType == m_modelSubType;
}

bool Common::ConfigModelSpecRegression::operator!=(const Common::ConfigModelSpec &other) const
{
    return !(*this == other);
}
