//
// Created by Alberto Campi on 24/07/2019.
//

#ifndef WILDCATSTKCORE_TOOLS_H
#define WILDCATSTKCORE_TOOLS_H

#include <boost/algorithm/string.hpp>
#include <vector>
#include <unordered_map>

namespace Common
{
    //StringSplit class declaration
    class StringSplit
    {
    public:
        std::vector<std::string> split(const std::string &string, const std::string &pattern) const;
    };


    //StringSplit decorator declarations
    class StringSplitDecorator
    {
    public:
        StringSplitDecorator();

        virtual void split(const std::string &string, const std::string &pattern) = 0;
        std::vector<std::string> get() const;

        virtual ~StringSplitDecorator() = default;

    protected:
        std::vector<std::string> m_components;
        Common::StringSplit m_strplit;
    };


    class StringSplitConfigVariableDecorator : public StringSplitDecorator
    {
    public:
        void split(const std::string &string, const std::string &pattern) final;

        std::string getBasename() const;
        std::string getTransformationTypeCode() const;
        unsigned int getLagDependency() const;
    };


    class StringSplitAlgebraicDecorator : public StringSplitDecorator
    {
    public:
        void splitExpression(const std::string &string);
        std::vector<std::string> getOrderedOperators() const;

    private:
        void split(const std::string &string, const std::string &pattern) final;
        static bool _isOperator(const std::string& string);
        bool _isValidExpression() const;
        std::vector<std::string> m_operators;
    };


    class AlgebraicExpressionParser
    {
    public:
        AlgebraicExpressionParser(const std::string& expression);

        void setExpression(const std::string& expression);
        std::vector<std::string> getComponents() const;

        double evaluate(const std::unordered_map<std::string, double> variableValuePairs) const;

    private:
        Common::StringSplitAlgebraicDecorator m_strsplit;
    };

    //Tool global functions : may be re-organized into separate header or class at a later stage
    double getTenorInYearsFromVariableName(const std::string& variableName);
}

#endif //WILDCATSTKCORE_TOOLS_H
