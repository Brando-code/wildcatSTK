//
// Created by Alberto Campi on 22/07/2019.
//

#ifndef WILDCATSTKCORE_CONFIGVARIABLE_H
#define WILDCATSTKCORE_CONFIGVARIABLE_H

#include <string>
#include <memory>
#include "../Common.h"

class TimeSeries;
class TransformationType;

class Common::ConfigVariable
{
public:
    ConfigVariable(const std::string& rawConfigVariable);

    std::string getBasename() const;
    std::string getTransformationTypeCode() const;
    unsigned int getLagDependency() const;
    TransformationType getTransformationType() const;

    double getTransformedVariableValue(const TimeSeries& ts, int index) const;
    double getLevel(const TimeSeries& ts, double transformedValue, int index) const;

private:
    const std::string m_basename, m_transformationCode;
    const std::unique_ptr<TransformationType> m_transformationTypePtr;

};


class Common::TransformationType
{
public:
    virtual double getTransformedVariableValue(const TimeSeries& ts, int index) const = 0;
    virtual double getLevel(const TimeSeries& ts, double transformedValue, int index) const = 0;

    virtual ~TransformationType() = default;
};


class Common::FirstDifferenceTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, int index) const final;
};


class Common::SimpleReturnTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, int index) const final;
};


class Common::LogReturnTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, int index) const final;
};


class Common::LevelTransformation : public Common::TransformationType
{
    double getTransformedVariableValue(const TimeSeries& ts, int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, int index) const final;
};

//[AC] consider switching to abstract factory + mapping
class Common::TransformationTypeFactory
{
    static std::unique_ptr<TransformationType> create(const std::string& transformationTypeCode);
};


#endif //WILDCATSTKCORE_CONFIGVARIABLE_H
