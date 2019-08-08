//
// Created by Alberto Campi on 22/07/2019.
//

#include "ConfigVariable.h"
#include "../../Global/Mappings/FactoryMappings.h"
#include "../Types/TimeSeries.h"
#include <cmath>
#include <utility>

using namespace Common;

//TransformationType class hierarchy implementation
double FirstDifferenceTransformation::getTransformedValue(const TimeSeries &ts, unsigned int index) const
{
    return ts.getValue(index) - ts.getValue(index - 1);
}

double FirstDifferenceTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return transformedValue + ts.getValue(index - 1);
}

std::unique_ptr<Common::TransformationType> FirstDifferenceTransformation::clone() const
{
    return std::make_unique<Common::FirstDifferenceTransformation>(*this);
}

double SimpleReturnTransformation::getTransformedValue(const TimeSeries &ts, unsigned int index) const
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

std::unique_ptr<Common::TransformationType> SimpleReturnTransformation::clone() const
{
    return std::make_unique<Common::SimpleReturnTransformation>(*this);
}

double LogReturnTransformation::getTransformedValue(const TimeSeries &ts, unsigned int index) const
{
    return log(ts.getValue(index)) - log(ts.getValue(index - 1));
}

double LogReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const
{
    return exp(transformedValue) * ts.getValue(index - 1);
}

std::unique_ptr<Common::TransformationType> LogReturnTransformation::clone() const
{
    return std::make_unique<Common::LogReturnTransformation>(*this);
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
ConfigVariable::ConfigVariable(const std::string &rawConfigVariable, std::string  delimiter) : m_strsplit(), m_delimiter(std::move(delimiter))
{
    m_strsplit.split(rawConfigVariable, m_delimiter);
    m_transformationTypePtr = Global::TransformationTypeCodeFactoryMapping::instance() ->
            getFactory(m_strsplit.getTransformationTypeCode()) -> create();
}

ConfigVariable::ConfigVariable(const std::string &rawConfigVariable) : ConfigVariable(rawConfigVariable, "|") {}

ConfigVariable::ConfigVariable(const Common::ConfigVariable &other) :
        m_strsplit(other.m_strsplit), m_delimiter(other.m_delimiter),
        m_transformationTypePtr(other.m_transformationTypePtr -> clone()) {}

ConfigVariable::ConfigVariable(Common::ConfigVariable &&other) :
        m_strsplit(other.m_strsplit), m_delimiter(other.m_delimiter),
        m_transformationTypePtr(std::move(other.m_transformationTypePtr)) {}

ConfigVariable& ConfigVariable::operator=(const Common::ConfigVariable &other)
{
    if (&other != this)
    {
        m_strsplit = other.m_strsplit;
        m_delimiter = other.m_delimiter;
        m_transformationTypePtr = other.m_transformationTypePtr -> clone();
    }
    return *this;
}

ConfigVariable& ConfigVariable::operator=(Common::ConfigVariable &&other)
{
    if (&other != this)
    {
        m_strsplit = other.m_strsplit;
        m_delimiter = other.m_delimiter;
        m_transformationTypePtr = std::move(other.m_transformationTypePtr);
    }
    return *this;
}

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

double ConfigVariable::getTransformedValue(const Common::TimeSeries &ts, unsigned int index) const
{
    const unsigned int lag = m_strsplit.getLagDependency();
    if (m_transformationTypePtr)
        return m_transformationTypePtr -> getTransformedValue(ts, index - lag);
    else
        return ts.getValue(index - lag);
}

double ConfigVariable::getLevel(const Common::TimeSeries &ts, double transformedValue, unsigned int index) const
{
    const unsigned int lag = m_strsplit.getLagDependency();
    if (m_transformationTypePtr)
        return m_transformationTypePtr -> getLevel(ts, transformedValue, index - lag);
    else
        return transformedValue;
}

std::vector<double> ConfigVariable::getTransformedTimeSeriesValues(const Common::TimeSeries &ts) const
{
    const unsigned int firstValidIndex = m_strsplit.getLagDependency() + 1;
    std::vector<double> transformedValues;

    for (unsigned int i = firstValidIndex; i < ts.getValues().size(); ++i)
        transformedValues.push_back(getTransformedValue(ts, i));

    return transformedValues;
}

bool ConfigVariable::operator==(const Common::ConfigVariable &other) const
{
    return m_strsplit.getBasename() == other.m_strsplit.getBasename() and
           m_strsplit.getTransformationTypeCode() == other.m_strsplit.getTransformationTypeCode() and
           m_strsplit.getLagDependency() == other.m_strsplit.getLagDependency() and
           m_delimiter == other.m_delimiter;
}

bool ConfigVariable::operator!=(const Common::ConfigVariable &other) const
{
    return !(*this == other);
}