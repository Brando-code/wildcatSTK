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
ConfigVariable::ConfigVariable(const std::string &rawConfigVariable)
{
    const std::string delimiter = "|";
    std::vector<std::string> splitConfigVariable;
    //[delimiter](char c[]){ return c == delimiter;}
    boost::split(splitConfigVariable, rawConfigVariable, boost::is_any_of(delimiter));

    const unsigned int expectedConfigVariableComponents = 3; //rawConfigVariable = basename|transformationTypeCode|lag
    //const unsigned int minimumConfigVariableComponents = 1; //rawConfigVariable = basename (curve models only)
    if (splitConfigVariable.size() != expectedConfigVariableComponents)
        throw std::runtime_error("E: .ConfigVariable::ConfigVariable : unexpected number of components for raw "
                                 "config variable " + rawConfigVariable);
    else
        {
            m_basename = splitConfigVariable[0];
            m_transformationCode = splitConfigVariable[1];
            m_lag = static_cast<unsigned int>(atoi(splitConfigVariable[2].c_str()));
            m_transformationTypePtr = Global::TransformationTypeCodeFactoryMapping::instance() ->
                    getMapping().at(m_transformationCode) -> create();
        }
}

std::string ConfigVariable::getBasename() const
{
    return m_basename;
}

std::string ConfigVariable::getTransformationTypeCode() const
{
    return m_transformationCode;
}

unsigned int ConfigVariable::getLagDependency() const
{
    return m_lag;
}

double ConfigVariable::getTransformedVariableValue(const Common::TimeSeries &ts, unsigned int index) const
{
    return m_transformationTypePtr -> getTransformedVariableValue(ts, index - m_lag);
}

double ConfigVariable::getLevel(const Common::TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return m_transformationTypePtr -> getLevel(ts, transformedValue, index - m_lag);
}




