//
// Created by Alberto Campi on 22/07/2019.
//

#include "ConfigVariable.h"
#include "../../Global/Mappings/FactoryMappings.h"
#include <cmath>
#include <boost/algorithm/string.hpp>

using namespace Common;

//TransformationType class hierarchy implementation
double FirstDifferenceTransformation::getTransformedVariableValue(const TimeSeries &ts, unsigned int index) const
{
    return ts.getValues().at(index) - ts.getValues().at(index - 1);
}

double FirstDifferenceTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return transformedValue + ts.getValues().at(index - 1);
}

double SimpleReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, unsigned int index) const
{
    if (ts.getValues().at(index - 1) != 0)
        return ts.getValues().at(index)/ts.getValues().at(index - 1) - 1;
    else
        throw std::overflow_error("E: SimpleReturnTransformation::getTransformedVariableValue : Division by zero error for variable " +
        ts.getName() + " at index " + std::to_string(index));
}

double SimpleReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return ts.getValues().at(index - 1) * (1 + transformedValue);
}

double LogReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, unsigned int index) const
{
    return log(ts.getValues().at(index)) - log(ts.getValues().at(index - 1));
}

double LogReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return exp(transformedValue) * ts.getValues().at(index - 1);
}

double LevelTransformation::getTransformedVariableValue(const TimeSeries &ts, unsigned int index) const
{
    return ts.getValues().at(index);
}

double LevelTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return ts.getValues().at(index);
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
    return std::make_unique<LevelTransformation>(LevelTransformation());
}


//ConfigVariable class implementation
ConfigVariable::ConfigVariable(const std::string &rawConfigVariable, const std::string& delimiter) : m_strsplit(), m_delimiter(delimiter)
{
    m_strsplit.split(rawConfigVariable, m_delimiter);
    m_transformationTypePtr = Global::TransformationTypeCodeFactoryMapping::instance() ->
            getMapping().at(m_strsplit.getTransformationTypeCode()) -> create();
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
    return m_transformationTypePtr -> getTransformedVariableValue(ts, index - lag);
}

double ConfigVariable::getLevel(const Common::TimeSeries &ts, double transformedValue, unsigned int index) const
{
    const unsigned int lag = m_strsplit.getLagDependency();
    return m_transformationTypePtr -> getLevel(ts, transformedValue, index - lag);
}




