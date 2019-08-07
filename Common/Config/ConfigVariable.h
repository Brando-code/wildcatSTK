//
// Created by Alberto Campi on 22/07/2019.
//

#ifndef WILDCATSTKCORE_CONFIGVARIABLE_H
#define WILDCATSTKCORE_CONFIGVARIABLE_H

#include <string>
#include <memory>
//#include "../Common.h"
#include "../Concepts.h"
#include "../Utils/General/Tools.h"

namespace Common
{
    class TimeSeries;
    class TransformationType;

    //ConfigVariable class declaration
    class ConfigVariable //: private Uncopyable
    {
    public:
        explicit ConfigVariable(const std::string &rawConfigVariable);
        ConfigVariable(const std::string &rawConfigVariable, std::string delimiter);
        ConfigVariable(const ConfigVariable &other);
        ConfigVariable& operator=(const ConfigVariable& other);

        std::string getBasename() const;
        std::string getTransformationTypeCode() const;
        unsigned int getLagDependency() const;
        double getTransformedValue(const TimeSeries &ts, unsigned int index) const;
        double getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const;
        std::vector<double> getTransformedTimeSeriesValues(const TimeSeries &ts) const;

        bool operator==(const Common::ConfigVariable &other) const;
        bool operator!=(const Common::ConfigVariable &other) const;

    private:
        Common::StringSplitConfigVariableDecorator m_strsplit;
        std::string m_delimiter;
        std::unique_ptr<Common::TransformationType> m_transformationTypePtr;
    };


    //TransformationType class hierarchy declaration
    class TransformationType
    {
    public:
        virtual double getTransformedValue(const TimeSeries &ts, unsigned int index) const = 0;
        virtual double getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const = 0;

        virtual std::unique_ptr<Common::TransformationType> clone() const = 0;

        virtual ~TransformationType() = default;
    };


    class FirstDifferenceTransformation : public TransformationType
    {
        double getTransformedValue(const TimeSeries &ts, unsigned int index) const final;
        double getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const final;

        std::unique_ptr<Common::TransformationType> clone() const final;
    };


    class SimpleReturnTransformation : public TransformationType
    {
        double getTransformedValue(const TimeSeries &ts, unsigned int index) const final;
        double getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const final;

        std::unique_ptr<Common::TransformationType> clone() const final;
    };


    class LogReturnTransformation : public TransformationType
    {
        double getTransformedValue(const TimeSeries &ts, unsigned int index) const final;
        double getLevel(const TimeSeries &ts, double transformedValue, unsigned int index) const final;

        std::unique_ptr<Common::TransformationType> clone() const final;
    };


    //Abstract factory class hierarchy declaration
    class TransformationTypeFactory
    {
    public:
        virtual std::unique_ptr<Common::TransformationType> create() const = 0;

        virtual ~TransformationTypeFactory() = default;
    };


    class FirstDifferenceTransformationFactory : public TransformationTypeFactory
    {
    public:
        std::unique_ptr<Common::TransformationType> create() const final;
    };


    class SimpleReturnTransformationFactory : public TransformationTypeFactory
    {
    public:
        std::unique_ptr<Common::TransformationType> create() const final;
    };


    class LogReturnTransformationFactory : public TransformationTypeFactory
    {
    public:
        std::unique_ptr<Common::TransformationType> create() const final;
    };


    class LevelTransformationFactory : public TransformationTypeFactory
    {
    public:
        std::unique_ptr<Common::TransformationType> create() const final;
    };

}

#endif //WILDCATSTKCORE_CONFIGVARIABLE_H
