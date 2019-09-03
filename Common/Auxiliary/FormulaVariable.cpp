//
// Created by Alberto Campi on 2019-08-25.
//

#include "FormulaVariable.h"
#include "../Types/DataSet.h"
#include "../Seasonality/SeasonalDecompose.h"
#include "../../Global/Mappings/FactoryMappings.h"

Common::FormulaVariableAlgebraic::FormulaVariableAlgebraic(const std::string &expression, const Common::OperatorsGrammar &grammar) :
    m_parser(expression, grammar)
{

}

double Common::FormulaVariableAlgebraic::evaluate(const Common::DataSet &ds, const boost::gregorian::date &date) const
{
    const std::vector<std::string> variableNames = m_parser.getExpressionVariables();
    std::unordered_map<std::string, double> kvp;
    for (const auto& variable: variableNames)
        kvp.emplace(variable, ds.getValue(variable, date));

    return m_parser.evaluate(Common::AlgebraicExpressionContext(kvp));
}

void Common::FormulaVariableAlgebraic::set(const std::string &expression, const Common::OperatorsGrammar &grammar)
{
    m_parser.setOperatorGrammar(grammar), m_parser.setExpression(expression);
}

Common::FormulaVariableFunctionalDeSeason::FormulaVariableFunctionalDeSeason(const std::string &variableName,
                                                                             const std::string &decompositionType,
                                                                             unsigned int period) :
    m_variable(variableName),
    m_decompPtr(Global::SeasonalDecomposeFactoryMapping::instance() -> getFactory(decompositionType) -> create(period)),
    m_isDecomposed(false)
{

}

void Common::FormulaVariableFunctionalDeSeason::_decompose(const Common::DataSet &ds) const
{
    if (!m_isDecomposed)
    {
        m_decompPtr -> decompose(ds.getTimeSeries(m_variable));
        m_isDecomposed = true;
    }
}

double Common::FormulaVariableFunctionalDeSeason::evaluate(const Common::DataSet &ds, const boost::gregorian::date &date) const
{
    _decompose(ds);
    return (m_decompPtr -> getTrend() + m_decompPtr -> getNoise()).getValue(date);
}

std::vector<double> Common::FormulaVariableFunctionalDeSeason::getLastSeasonalCycle(const Common::DataSet &ds) const
{
    _decompose(ds);
    const unsigned int cycleLength = m_decompPtr -> getPeriod();
    std::vector<double>::const_iterator begin = m_decompPtr -> getSeason().getValues().end() - cycleLength;
    std::vector<double>::const_iterator end = m_decompPtr -> getSeason().getValues().end();
    return std::vector<double>(begin, end);
}

void Common::FormulaVariableFunctionalDeSeason::set(const std::string &variableName, const std::string &decompositionType,
                                                    unsigned int period)
{
    m_variable = variableName;
    m_decompPtr = Global::SeasonalDecomposeFactoryMapping::instance() -> getFactory(decompositionType) -> create(period);
    m_isDecomposed = false;
}