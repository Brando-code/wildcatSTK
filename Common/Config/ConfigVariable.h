//
// Created by Alberto Campi on 22/07/2019.
//

#ifndef WILDCATSTKCORE_CONFIGVARIABLE_H
#define WILDCATSTKCORE_CONFIGVARIABLE_H

#include <string>
#include <memory>
#include "../Common.h"
#include "../Utils/General/Tools.h"
#include "../Types/TimeSeries.h"


//ConfigVariable class declaration
class Common::ConfigVariable
{
public:
    explicit ConfigVariable(const std::string& rawConfigVariable);
    ConfigVariable(const std::string& rawConfigVariable, const std::string& delimiter);
    ConfigVariable(const ConfigVariable& other);
    //ConfigVariable& operator=(const ConfigVariable& other);

    std::string getBasename() const;
    std::string getTransformationTypeCode() const;
    unsigned int getLagDependency() const;

    double getTransformedValue(const TimeSeries& ts, unsigned int index) const;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const;
    std::vector<double> getTransformedTimeSeriesValues(const TimeSeries& ts) const;

    bool operator==(const Common::ConfigVariable& other) const;
    bool operator!=(const Common::ConfigVariable& other) const;

private:
    Common::StringSplitConfigVariableDecorator m_strsplit;
    std::string m_delimiter;
    std::unique_ptr<Common::TransformationType> m_transformationTypePtr;

};


//TransformationType class hierarchy declaration
class Common::TransformationType
{
public:
    virtual double getTransformedValue(const TimeSeries& ts, unsigned int index) const = 0;
    virtual double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const = 0;

    virtual std::unique_ptr<Common::TransformationType> clone() const = 0;

    virtual ~TransformationType() = default;
};


class Common::FirstDifferenceTransformation : public Common::TransformationType
{
    double getTransformedValue(const TimeSeries& ts, unsigned int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const final;

    std::unique_ptr<Common::TransformationType> clone() const final;
};


class Common::SimpleReturnTransformation : public Common::TransformationType
{
    double getTransformedValue(const TimeSeries& ts, unsigned int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const final;

    std::unique_ptr<Common::TransformationType> clone() const final;
};


class Common::LogReturnTransformation : public Common::TransformationType
{
    double getTransformedValue(const TimeSeries& ts, unsigned int index) const final;
    double getLevel(const TimeSeries& ts, double transformedValue, unsigned int index) const final;

    std::unique_ptr<Common::TransformationType> clone() const final;
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
