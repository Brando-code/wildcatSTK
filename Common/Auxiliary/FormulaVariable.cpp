//
// Created by Alberto Campi on 2019-08-25.
//

#include "FormulaVariable.h"
#include "../Types/DataSet.h"

Common::FormulaVariableAlgebraic::FormulaVariableAlgebraic(const std::string &expression, const Common::OperatorsGrammar &grammar) :
    m_parser(expression, grammar)
{}

double Common::FormulaVariableAlgebraic::evaluate(const Common::DataSet &ds, const boost::gregorian::date &date) const
{
    const std::vector<std::string> variableNames = m_parser.getExpressionVariables();
    std::unordered_map<std::string, double> kvp;
    for (const auto& variable: variableNames)
        kvp.emplace(variable, ds.getValue(variable, date));

    return m_parser.evaluate(Common::AlgebraicExpressionContext(kvp));
}