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

}

#endif //WILDCATSTKCORE_FORMULAVARIABLE_H
