//
// Created by Alberto Campi on 27/07/2019.
//

#include "ConfigModelSpec.h"
#include "../Types/DataSet.h"
#include "../../Global/Mappings/FactoryMappings.h"

//ConfigModelSpec class implementation
Common::ConfigModelSpec::ConfigModelSpec(const Common::ConfigVariable &dependentVariable,
                                         const std::vector<Common::ConfigVariable> &independentVariables) :
        m_dVariable(dependentVariable),
        m_idVariables(independentVariables),
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
