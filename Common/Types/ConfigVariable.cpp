//
// Created by Alberto Campi on 22/07/2019.
//

#include "ConfigVariable.h"
#include "../../Global/Mappings/FactoryMappings.h"
#include <cmath>
#include <boost/algorithm/string.hpp>

using namespace Common;

//TransformationType class hierarchy implementation
double FirstDifferenceTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    return ts.getValues().at(index) - ts.getValues().at(index - 1);
}

double FirstDifferenceTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return transformedValue + ts.getValues().at(index - 1);
}

double SimpleReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    if (ts.getValues().at(index - 1) != 0)
        return ts.getValues().at(index)/ts.getValues().at(index - 1) - 1;
    else
        throw std::overflow_error("E: SimpleReturnTransformation::getTransformedVariableValue : Division by zero error for variable " +
        ts.getName() + " at index " + std::to_string(index));
}

double SimpleReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return ts.getValues().(index - 1) * (1 + transformedValue);
}

double LogReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    return log(ts.getValues().(index)) - log(ts.getValues().(index - 1));
}

double LogReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return exp(transformedValue) * ts.getValues().(index - 1);
}

double LevelTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    return ts.getValues().(index);
}

double LevelTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return ts.getValues().(index);
}


//TransformationType class hierarchy implementation
std::unique_ptr<TransformationType> FirstDifferenceTransformationFactory::create() const
{
    return std::make_unique<TransformationType>(FirstDifferenceTransformation());
}

std::unique_ptr<TransformationType> SimpleReturnTransformationFactory::create() const
{
    return std::make_unique<TransformationType>(SimpleReturnTransformation());
}

std::unique_ptr<TransformationType> LogReturnTransformationFactory::create() const
{
    return std::make_unique<TransformationType>(LogReturnTransformation());
}

std::unique_ptr<TransformationType> LevelTransformationFactory::create() const
{
    return std::make_unique<TransformationType>(LevelTransformation());
}


//ConfigVariable class implementation
ConfigVariable::ConfigVariable(const std::string &rawConfigVariable)
{
    const char* delimiter = "|";
    std::vector<std::string> splitConfigVariable;
    boost::split(splitConfigVariable, rawConfigVariable, [](const char* c){ return c == delimiter;});

    const unsigned int expectedConfigVariableComponents = 3; //rawConfigVariable = basename|transformationTypeCode|lag
    //const unsigned int minimumConfigVariableComponents = 1; //rawConfigVariable = basename (curve models only)
    if (splitConfigVariable.size() != expectedConfigVariableComponents)
        throw std::runtime_error("E: .ConfigVariable::ConfigVariable : unexpected number of components for raw "
                                 "config variable " + rawConfigVariable);
    else
        {
            m_basename = splitConfigVariable[0];
            m_transformationCode = splitConfigVariable[1];
            m_lag = static_cast<unsigned int>(atol(splitConfigVariable[2].c_str()));
            m_transformationTypePtr = Global::TransformationTypeCodeFactoryMapping::instance() ->
                    getMapping().at(m_transformationCode).create();
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

double ConfigVariable::getTransformedVariableValue(const Common::TimeSeries &ts, int index) const
{
    return m_transformationTypePtr -> getTransformedVariableValue(ts, index);
}

double ConfigVariable::getLevel(const Common::TimeSeries &ts, double transformedValue, int index) const
{
    return m_transformationTypePtr -> getLevel(ts, transformedValue, index);
}




