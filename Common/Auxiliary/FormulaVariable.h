//
// Created by Alberto Campi on 2019-08-25.
//

#ifndef WILDCATSTKCORE_FORMULAVARIABLE_H
#define WILDCATSTKCORE_FORMULAVARIABLE_H

#include <boost/date_time/gregorian/gregorian.hpp>
#include "../Utils/General/Tools.h"

namespace Common
{
    class DataSet;
    class SeasonalDecompose;

    class FormulaVariable
    {
    public:
        virtual double evaluate(const Common::DataSet& ds, const boost::gregorian::date& date) const = 0;

        virtual ~FormulaVariable() = default;
    };

    class FormulaVariableAlgebraic : public FormulaVariable
    {
    public:
        FormulaVariableAlgebraic(const std::string& expression, const Common::OperatorsGrammar& grammar);
        double evaluate(const Common::DataSet& ds, const boost::gregorian::date& date) const final;

    private:
        mutable Common::AlgebraicExpressionParser m_parser;
    };

    class FormulaVariableFunctionalDeSeason : public FormulaVariable
    {
    public:
        FormulaVariableFunctionalDeSeason(const std::string& variableName, const std::string& decompositionType, unsigned int period);
        double evaluate(const Common::DataSet& ds, const boost::gregorian::date& date) const final;

    private:
        std::string m_variable;
        std::unique_ptr<Common::SeasonalDecompose> m_decompPtr;
    };

}

#endif //WILDCATSTKCORE_FORMULAVARIABLE_H
