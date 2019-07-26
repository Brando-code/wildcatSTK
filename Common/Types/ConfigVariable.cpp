//
// Created by Alberto Campi on 22/07/2019.
//

#include "ConfigVariable.h"
#include "../../Global/Mappings/FactoryMappings.h"
#include <cmath>

using namespace Common;

//TransformationType class hierarchy implementation
double FirstDifferenceTransformation::getTransformedVariableValue(const TimeSeries &ts, unsigned int index) const
{
    return ts.getValue(index) - ts.getValue(index - 1);
}

double FirstDifferenceTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return transformedValue + ts.getValue(index - 1);
}

double SimpleReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, unsigned int index) const
{
    if (ts.getValue(index - 1) != 0)
        return ts.getValue(index)/ts.getValue(index - 1) - 1;
    else
        throw std::overflow_error("E: SimpleReturnTransformation::getTransformedVariableValue : Division by zero error for variable " +
        ts.getName() + " at index " + std::to_string(index));
}

double SimpleReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return ts.getValue(index - 1) * (1 + transformedValue);
}

double LogReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, unsigned int index) const
{
    return log(ts.getValue(index)) - log(ts.getValue(index - 1));
}

double LogReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return exp(transformedValue) * ts.getValue(index - 1);
}


//TransformationType class hierarchy implementation
std::unique_ptr<TransformationType> FirstDifferenceTransformationFactory::create() const
{
    return std::make_unique<FirstDifferenceTransformation>(FirstDifferenceTransformation());
}

std::unique_ptr<TransformationType> SimpleReturnTransformationFactory::create() const
{
    return std::make_unique<SimpleReturnTransformation>(SimpleReturnTransformation());
}

std::unique_ptr<TransformationType> LogReturnTransformationFactory::create() const
{
    return std::make_unique<LogReturnTransformation>(LogReturnTransformation());
}

std::unique_ptr<TransformationType> LevelTransformationFactory::create() const
{
    //return std::make_unique<LevelTransformation>(LevelTransformation());
    return nullptr;
}


//ConfigVariable class implementation
ConfigVariable::ConfigVariable(const std::string &rawConfigVariable, const std::string& delimiter) : m_strsplit(), m_delimiter(delimiter)
{
    m_strsplit.split(rawConfigVariable, m_delimiter);
    m_transformationTypePtr = Global::TransformationTypeCodeFactoryMapping::instance() ->
            getFactory(m_strsplit.getTransformationTypeCode()) -> create();
}

ConfigVariable::ConfigVariable(const std::string &rawConfigVariable) : ConfigVariable(rawConfigVariable, "|") {}

std::string ConfigVariable::getBasename() const
{
    return m_strsplit.getBasename();
}

std::string ConfigVariable::getTransformationTypeCode() const
{
    return m_strsplit.getTransformationTypeCode();
}

unsigned int ConfigVariable::getLagDependency() const
{
    return m_strsplit.getLagDependency();
}

double ConfigVariable::getTransformedVariableValue(const Common::TimeSeries &ts, unsigned int index) const
{
    const unsigned int lag = m_strsplit.getLagDependency();
    if (m_transformationTypePtr)
        return m_transformationTypePtr -> getTransformedVariableValue(ts, index - lag);
    else
        return ts.getValue(index - lag);
}

double ConfigVariable::getLevel(const Common::TimeSeries &ts, double transformedValue, unsigned int index) const
{
    const unsigned int lag = m_strsplit.getLagDependency();
    if (m_transformationTypePtr)
        return m_transformationTypePtr -> getLevel(ts, transformedValue, index - lag);
    else
        return ts.getValue(index - lag);
}




