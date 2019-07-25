//
// Created by Alberto Campi on 22/07/2019.
//

#ifndef WILDCATSTKCORE_CONFIGVARIABLE_H
#define WILDCATSTKCORE_CONFIGVARIABLE_H

#include <string>
#include <memory>
#include "../Common.h"
#include "../Utils/Tools.h"
#include "TimeSeries.h"


//ConfigVariable class declaration
class Common::ConfigVariable
{
public:
    explicit ConfigVariable(const std::string& rawConfigVariable);
    ConfigVariable(const std::string& rawConfigVariable, const std::string& delimiter);

    std::string getBasename() const;
    std::string getTransformationTypeCode() const;
    unsigned int getLagDependency() const;

    double getTransformedVariableValue(const TimeSeries& ts, unsigned int index) const;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const;

private:    //[AC] find a way to initialize class from initialization list and mark as const
    Common::StringSplitConfigVariableDecorator m_strsplit;
    const std::string m_delimiter;
    std::unique_ptr<Common::TransformationType> m_transformationTypePtr;

};


//TransformationType class hierarchy declaration
class Common::TransformationType
{
public:
    virtual double getTransformedVariableValue(const TimeSeries& ts, unsigned int index) const = 0;
    virtual double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const = 0;

    virtual ~TransformationType() = default;
};


class Common::FirstDifferenceTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, unsigned int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const final;
};


class Common::SimpleReturnTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, unsigned int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const final;
};


class Common::LogReturnTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, unsigned int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const final;
};


class Common::LevelTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, unsigned int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const final;
};


//Abstract factory class hierarchy declaration
class Common::TransformationTypeFactory
{
public:
    virtual std::unique_ptr<Common::TransformationType> create() const = 0;
    virtual ~TransformationTypeFactory() = default;
};


class Common::FirstDifferenceTransformationFactory : public TransformationTypeFactory
{
public:
    std::unique_ptr<Common::TransformationType> create() const final;
};


class Common::SimpleReturnTransformationFactory : public TransformationTypeFactory
{
public:
    std::unique_ptr<Common::TransformationType> create() const final;
};


class Common::LogReturnTransformationFactory : public TransformationTypeFactory
{
public:
    std::unique_ptr<Common::TransformationType> create() const final;
};


class Common::LevelTransformationFactory : public TransformationTypeFactory
{
public:
    std::unique_ptr<Common::TransformationType> create() const final;
};


#endif //WILDCATSTKCORE_CONFIGVARIABLE_H
