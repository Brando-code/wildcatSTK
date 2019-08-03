//
// Created by Alberto Campi on 27/07/2019.
//

#include "ConfigModelSpec.h"

#include <utility>
#include <boost/numeric/ublas/matrix.hpp>
#include "../Types/DataSet.h"
#include "../../Global/Mappings/FactoryMappings.h"

//ConfigModelSpec class implementation
Common::ConfigModelSpec::ConfigModelSpec(const Common::ConfigVariable &dependentVariable,
                                         std::vector<Common::ConfigVariable> independentVariables) :
        m_dVariable(dependentVariable),
        m_idVariables(std::move(independentVariables)),
        m_coeff() {}

Common::ConfigVariable Common::ConfigModelSpec::getDependentVariable() const
{
    return m_dVariable;
}

std::vector<Common::ConfigVariable> Common::ConfigModelSpec::getIndependentVariables() const
{
    return m_idVariables;
}


//ConfigModelSpecRelative class implementation
Common::ConfigModelSpecRelative::ConfigModelSpecRelative(const Common::ConfigVariable &dependentVariable,
                                                         const std::vector<Common::ConfigVariable> &independentVariables,
                                                         const std::string &modelSubType, int multiplier) :
        ConfigModelSpec(dependentVariable, independentVariables), m_multiplier(multiplier)
{
    //initialize m_modelPtr via factory
    m_modelPtr = Global::RelativeModelFactoryMapping::instance() -> getFactory(modelSubType) -> create();
}

void Common::ConfigModelSpecRelative::calibrate(const Common::DataSet &ds)
{
    if (m_idVariables.size() > 1)   //[AC] maybe we should throw?
        std::cerr << "W: ConfigModelSpecRelative::calibrate : variable "
                     << m_dVariable.getBasename() <<  " has multiple drivers for relative model type. Using first driver only..." << std::endl;

    const std::vector<double> transformedDependentVariableValues = m_dVariable.getTransformedTimeSeriesValues(ds.getTimeSeries(m_dVariable.getBasename()));
    const std::vector<double> transformedIndependentVariableValues = m_idVariables[0].getTransformedTimeSeriesValues(ds.getTimeSeries(m_idVariables[0].getBasename()));
    
    //delegate execution to m_modelPtr
    m_modelPtr -> calibrate(m_coeff, transformedDependentVariableValues, transformedIndependentVariableValues);
}

double Common::ConfigModelSpecRelative::predict(const Common::DataSet &ds, unsigned int index) const
{
    const double transformedProjection = m_coeff[0] * m_idVariables[0].getTransformedValue(ds.getTimeSeries(m_idVariables[0].getBasename()), index);
    return m_dVariable.getLevel(ds.getTimeSeries(m_dVariable.getBasename()), transformedProjection, index);
}


//ConfigModelSpecRegression class implementation
Common::ConfigModelSpecRegression::ConfigModelSpecRegression(const Common::ConfigVariable &dependentVariable,
                                                             const std::vector<Common::ConfigVariable> &independentVariables,
                                                             const std::string &modelSubType,
                                                             const boost::gregorian::date &regressionStartDate) :
        ConfigModelSpec(dependentVariable, independentVariables), m_startDate(regressionStartDate) //initialize pointer too
{}

void Common::ConfigModelSpecRegression::calibrate(const Common::DataSet &ds)
{
    //get first available date across drivers and dependent variable
    boost::gregorian::date firstAvailableDate = ds.getTimeSeries(m_dVariable.getBasename()).getDates().at(0);
    for (const auto& it: m_idVariables)
    {
        boost::gregorian::date thisDriverFirstAvailableDate = ds.getTimeSeries(it.getBasename()).getDates().at(0);
        if (thisDriverFirstAvailableDate < firstAvailableDate)
            firstAvailableDate = thisDriverFirstAvailableDate;
    }

    //check that the regression start date is more recent than the first available start date
    if (m_startDate > firstAvailableDate)
        firstAvailableDate = m_startDate;

    //construct array of dependent variable values to be used by MLRegression
    const unsigned long dVariableFirstIndex = ds.getTimeSeries(m_dVariable.getBasename()).getIndex(firstAvailableDate);
    std::vector<double>::const_iterator begin = ds.getTimeSeries(m_dVariable.getBasename()).getValues().begin() + dVariableFirstIndex;
    std::vector<double>::const_iterator end = ds.getTimeSeries(m_dVariable.getBasename()).getValues().end();
    const std::vector<double> dVariableValuesForRegression(begin, end);

    //construct matrix of independent variable values to be used by MLRegression
    const unsigned long nRows = ds.getTimeSeries(m_dVariable.getBasename()).getDates().size() - dVariableFirstIndex;
    const unsigned long nCols = m_idVariables.size() + 1;
    boost::numeric::ublas::matrix<double> idVariableValuesForRegression(nRows, nCols);

    for (unsigned long i = 0; i < nCols; ++i)
    {
        const unsigned int idVariableFirstIndex = ds.getTimeSeries(m_idVariables.at(i).getBasename()).getIndex(firstAvailableDate);
        for (unsigned long j = 0; j < nRows; ++j)
        {
            //fill in matrix with values
            const double value = ds.getTimeSeries(m_idVariables.at(i).getBasename()).getValue(j + idVariableFirstIndex);
            idVariableValuesForRegression(i, j) = value;
        }
    }

    //delegate execution to RegressionModelObject
    //m_modelPtr -> calibrate(dVariableValuesForRegression, idVariableValuesForRegression, m_params);
}

double Common::ConfigModelSpecRegression::predict(const Common::DataSet &ds, unsigned int index) const
{
    return 0;
}