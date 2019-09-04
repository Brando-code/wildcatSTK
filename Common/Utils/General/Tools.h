//
// Created by Alberto Campi on 24/07/2019.
//

#ifndef WILDCATSTKCORE_TOOLS_H
#define WILDCATSTKCORE_TOOLS_H

#include <boost/algorithm/string.hpp>
#include <vector>
#include <unordered_map>
#include <queue>
#include "AlgebraicExpressionInterpreter.h"

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
        StringSplitAlgebraicDecorator(const Common::OperatorsGrammar& opGrammar);
        void splitExpression(const std::string &string);
        std::vector<std::string> getTokenized() const;

        void setOperatorsGrammar(const Common::OperatorsGrammar& opGrammar);

    private:
        void split(const std::string &string, const std::string &pattern) final;

        std::vector<std::string> m_tokenized;
        std::unique_ptr<Common::OperatorsGrammar> m_opGrammar;
    };


    class AlgebraicExpressionParser
    {
    public:
        AlgebraicExpressionParser(const std::string& expression, const Common::OperatorsGrammar& opGrammar);

        void setExpression(const std::string& other);
        void setOperatorGrammar(const Common::OperatorsGrammar& other);

        std::vector<std::string> getExpressionVariables() const;

        double evaluate(const Common::AlgebraicExpressionContext& context);

    private:
        std::unique_ptr<Common::OperatorsGrammar> m_opGrammar;
        std::vector<std::string> m_tokenized, m_variables;

        std::vector<std::string>::const_iterator m_itToken;
        std::vector<std::string>::const_iterator m_itExprEnd;

        std::unique_ptr<Common::AlgebraicExpression> _parsePrimary(void);
        std::unique_ptr<Common::AlgebraicExpression> _parseExpression(unsigned int lowestPrecedenceValue);
    };


    //Tool global functions : may be re-organized into separate header or class at a later stage
    double getTenorInYearsFromVariableName(const std::string& variableName);
    bool isNumeric(const std::string& string);
}

#endif //WILDCATSTKCORE_TOOLS_H
