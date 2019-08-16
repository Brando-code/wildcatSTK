//
// Created by Alberto Campi on 27/07/2019.
//

#include "ConfigModelSpec.h"

#include <utility>
#include <boost/numeric/ublas/matrix.hpp>
#include "../Types/DataSet.h"
#include "../Types/TimeSeries.h"
#include "../../Global/Mappings/FactoryMappings.h"
#include "../Math/MLRegression/RegressionModel.h"
#include "../Math/Relative/RelativeModel.h"


//ConfigModelSpec class implementation
Common::ConfigModelSpec::ConfigModelSpec(const Common::ConfigVariable &dependentVariable,
                        std::vector<Common::ConfigVariable> independentVariables) :
        m_dVariable(dependentVariable),
        m_idVariables(std::move(independentVariables)),
        m_coeff()
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

bool Common::ConfigModelSpec::_equal(const Common::ConfigModelSpec &other) const
{
    return (m_dVariable == other.m_dVariable and m_idVariables == other.m_idVariables);
}



//ConfigModelSpecRelative class implementation
Common::ConfigModelSpecRelative::ConfigModelSpecRelative(const Common::ConfigVariable &dependentVariable,
                                                         const std::vector<Common::ConfigVariable> &independentVariables,
                                                         const std::string &modelSubType, double multiplier) :
        ConfigModelSpec(dependentVariable, independentVariables), m_multiplier(multiplier), m_modelSubType(modelSubType)
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

    const std::vector<double> transformedDependentVariableValues = m_dVariable.getTransformedTimeSeriesValues(ds.getTimeSeries(m_dVariable.getBasename()));
    const std::vector<double> transformedIndependentVariableValues = m_idVariables[0].getTransformedTimeSeriesValues(ds.getTimeSeries(m_idVariables[0].getBasename()));
    
    //delegate execution to m_modelPtr
    m_coeff.clear();
    m_modelPtr -> calibrate(m_coeff, transformedDependentVariableValues, transformedIndependentVariableValues);
}

double Common::ConfigModelSpecRelative::predict(const Common::DataSet &ds, unsigned int index) const
{
    if (m_coeff.empty())
        throw std::out_of_range("E: ConfigModelSpecRelative::predict : projections cannot be generated from un-calibrated models.");

    const double scalar = m_multiplier * m_coeff[0];
    const double transformedProjection = scalar * m_idVariables[0].getTransformedValue(ds.getTimeSeries(m_idVariables[0].getBasename()), index);
    return m_dVariable.getLevel(ds.getTimeSeries(m_dVariable.getBasename()), transformedProjection, index);
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

//ConfigModelSpecRegression class implementation
Common::ConfigModelSpecRegression::ConfigModelSpecRegression(const Common::ConfigVariable &dependentVariable,
                                                             const std::vector<Common::ConfigVariable> &independentVariables,
                                                             const std::string &modelSubType,
                                                             const boost::gregorian::date &regressionStartDate) :
        ConfigModelSpec(dependentVariable, independentVariables),
        m_startDate(regressionStartDate),
        m_modelSubType(modelSubType),
        //Should be replace by factory when more regression models are available
        m_modelPtr(std::make_unique<Math::RegressionModelOLS>(Math::RegressionModelOLS())) {}

Common::ConfigModelSpecRegression::ConfigModelSpecRegression(const Common::ConfigModelSpecRegression &other) :
        ConfigModelSpec(other.m_dVariable, other.m_idVariables),
        m_startDate(other.m_startDate),
        m_modelSubType(other.m_modelSubType),
        m_modelPtr(other.m_modelPtr -> clone()) {}

Common::ConfigModelSpecRegression::ConfigModelSpecRegression(Common::ConfigModelSpecRegression &&other) :
        ConfigModelSpec(other.m_dVariable, other.m_idVariables),
        m_startDate(other.m_startDate),
        m_modelSubType(other.m_modelSubType),
        m_modelPtr(std::move(other.m_modelPtr)) {}

Common::ConfigModelSpecRegression& Common::ConfigModelSpecRegression::operator=(const Common::ConfigModelSpecRegression &other)
{
    if (&other != this)
    {
        m_dVariable = other.m_dVariable;
        m_idVariables = other.m_idVariables;
        m_coeff = other.m_coeff;
        m_startDate = other.m_startDate;
        m_modelSubType = other.m_modelSubType;
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
        m_coeff = other.m_coeff;
        m_startDate = other.m_startDate;
        m_modelSubType = other.m_modelSubType;
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
    //get first valid date across drivers and dependent variable
    boost::gregorian::date firstValidDate = ds.getTimeSeries(m_dVariable.getBasename()).getDates().at(0);
    for (const auto& it: m_idVariables)
    {
        boost::gregorian::date thisDriverFirstAvailableDate = ds.getTimeSeries(it.getBasename()).getDates().at(0);
        if (thisDriverFirstAvailableDate > firstValidDate)
            firstValidDate = thisDriverFirstAvailableDate;
    }

    //check that the regression start date is more recent than the first available start date
    const std::vector<boost::gregorian::date> dVariableDates = ds.getTimeSeries(m_dVariable.getBasename()).getDates();
    if (m_startDate > firstValidDate && std::find(dVariableDates.begin(), dVariableDates.end(), m_startDate) != dVariableDates.end())
        firstValidDate = m_startDate;
    else
        std::cerr << "W: ConfigModelSpecRegression::getFirstValidRegressionDate : start regression date for variable " + m_dVariable.getBasename() +
        " is not in the data intersection. Using first valid date..." << std::endl;

    return firstValidDate;
}

void Common::ConfigModelSpecRegression::calibrate(const Common::DataSet &ds)
{
    //get first available date across drivers and dependent variable
    const boost::gregorian::date firstValidDate = getFirstValidRegressionDate(ds);

    //construct array of dependent variable values to be used by MLRegression
    const unsigned long dVariableFirstIndex = ds.getTimeSeries(m_dVariable.getBasename()).getIndex(firstValidDate);
    std::vector<double>::const_iterator begin = ds.getTimeSeries(m_dVariable.getBasename()).getValues().begin() + dVariableFirstIndex;
    std::vector<double>::const_iterator end = ds.getTimeSeries(m_dVariable.getBasename()).getValues().end();
    const std::vector<double> dVariableValuesForRegression(begin, end);

    //construct matrix of independent variable values to be used by MLRegression
    const unsigned long nRows = ds.getTimeSeries(m_dVariable.getBasename()).getDates().size() - dVariableFirstIndex;
    const unsigned long nCols = m_idVariables.size();
    boost::numeric::ublas::matrix<double> idVariableValuesForRegression(nRows, nCols);

    for (unsigned long i = 0; i < nCols; ++i)
    {
        const unsigned int idVariableFirstIndex = ds.getTimeSeries(m_idVariables.at(i).getBasename()).getIndex(firstValidDate);
        for (unsigned long j = 0; j < nRows; ++j)
        {
            //fill in matrix with values
            const double value = ds.getTimeSeries(m_idVariables.at(i).getBasename()).getValue(j + idVariableFirstIndex);
            idVariableValuesForRegression(i, j) = value;
        }
    }

    //delegate execution to RegressionModelObject
    m_coeff.clear();
    //m_modelPtr -> calibrate(dVariableValuesForRegression, idVariableValuesForRegression, m_params);
}

double Common::ConfigModelSpecRegression::predict(const Common::DataSet &ds, unsigned int index) const
{
    if (m_coeff.empty())
        throw std::out_of_range("E: ConfigModelSpecRegression::predict : projections cannot be generated from un-calibrated models.");

    const double intercept = m_coeff[0];
    double dTransformedVariable = 0;
    for (unsigned int i = 0; i < m_idVariables.size(); ++i)
    {
        const Common::TimeSeries ts = ds.getTimeSeries(m_idVariables.at(i).getBasename());
        dTransformedVariable += m_coeff.at(i + 1) * m_idVariables.at(i).getTransformedValue(ts, index);
    }

    dTransformedVariable += intercept;
    const Common::TimeSeries ts = ds.getTimeSeries(m_dVariable.getBasename());
    return m_dVariable.getLevel(ts, dTransformedVariable, index);
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
