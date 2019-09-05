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

Common::TimeSeries Common::FormulaVariableFunctionalDeSeason::compute(const Common::DataSet &ds) const
{
    _decompose(ds);
    return m_decompPtr -> getDeseasoned();
}

Common::TimeSeries Common::FormulaVariableFunctionalDeSeason::getSeason(const Common::DataSet &ds) const
{
    _decompose(ds);
    return m_decompPtr -> getSeason();
}

void Common::FormulaVariableFunctionalDeSeason::set(const std::string &variableName, const std::string &decompositionType,
                                                    unsigned int period)
{
    m_variable = variableName;
    m_decompPtr = Global::SeasonalDecomposeFactoryMapping::instance() -> getFactory(decompositionType) -> create(period);
    m_isDecomposed = false;
}

Common::FormulaVariableFunctionalRestoreSeason::FormulaVariableFunctionalRestoreSeason(const std::string &seasonalVariableName,
                                                                                       const std::string &deSeasonedVariableName,
                                                                                       const std::string &decompositionType,
                                                                                       unsigned int period,
                                                                                       const Common::TimeSeries &seasonality) :
    m_seasonalVariable(seasonalVariableName),
    m_deSeasonedVariable(deSeasonedVariableName),
    m_restorePtr(Global::RestoreSeasonFactoryMapping::instance() -> getFactory(decompositionType) -> create()),
    m_lastSeasonalCycle(_getLastSeasonalCycle(seasonality, period))
{

}

void Common::FormulaVariableFunctionalRestoreSeason::set(const std::string &seasonalVariableName,
                                                         const std::string &deSeasonedVariableName,
                                                         const std::string &decompositionType,
                                                         unsigned int period,
                                                         const Common::TimeSeries &seasonality)
{
    m_seasonalVariable = seasonalVariableName;
    m_deSeasonedVariable = deSeasonedVariableName;
    m_restorePtr = Global::RestoreSeasonFactoryMapping::instance() -> getFactory(decompositionType) -> create();
    m_lastSeasonalCycle = _getLastSeasonalCycle(seasonality, period);
}

std::vector<double> Common::FormulaVariableFunctionalRestoreSeason::_getLastSeasonalCycle(const Common::TimeSeries &seasonality,
                                                                                          unsigned int period)
{
    m_restoreStartDate = seasonality.getDates().back();
    std::vector<double>::const_iterator begin = seasonality.getValues().end() - period;
    std::vector<double>::const_iterator end = seasonality.getValues().end();
    return std::vector<double>(begin, end);
}

Common::TimeSeries Common::FormulaVariableFunctionalRestoreSeason::compute(const Common::DataSet &ds) const
{
    const unsigned long index = ds.getTimeSeries(m_seasonalVariable).getIndex(m_restoreStartDate);
    std::vector<double> restoredValues;
    std::vector<boost::gregorian::date> restoredDates;

    for (unsigned long i = index; i < ds.getTimeSeries(m_seasonalVariable).length(); ++i)
    {
        const double trendValue = ds.getValue(m_seasonalVariable, i);
        double seasonalValue;
        if (index < m_lastSeasonalCycle.size())
            seasonalValue = m_lastSeasonalCycle.at(index);
        else
            seasonalValue = m_lastSeasonalCycle.at(index % m_lastSeasonalCycle.size());

        ds.getTimeSeries(m_seasonalVariable).pushBack(ds.getTimeSeries(m_seasonalVariable).getDates().at(i),
                                                      m_restorePtr -> restore(trendValue, seasonalValue));
    }

    return ds.getTimeSeries(m_seasonalVariable);
}
/*
double Common::FormulaVariableFunctionalRestoreSeason::evaluate(const Common::DataSet &ds, const boost::gregorian::date &date) const
{
    const unsigned long index = ds.getTimeSeries(m_variable).getIndex(date);
    const double trendValue = ds.getValue(m_variable, date);

    double seasonalValue;
    if (index < m_lastSeasonalCycle.size())
        seasonalValue = m_lastSeasonalCycle.at(index);
    else
        seasonalValue = m_lastSeasonalCycle.at(index % m_lastSeasonalCycle.size());

    return m_restorePtr -> restore(trendValue, seasonalValue);
}
*/
double Common::RestoreSeasonAdditive::restore(double deSeasonedValue, double seasonalValue) const
{
    return deSeasonedValue + seasonalValue;
}

double Common::RestoreSeasonMultiplicative::restore(double deSeasonedValue, double seasonalValue) const
{
    return deSeasonedValue * seasonalValue;
}


// Factory classes
std::unique_ptr<Common::RestoreSeason> Common::RestoreSeasonAdditiveFactory::create() const
{
    using namespace Common;
    return std::make_unique<RestoreSeasonAdditive>(RestoreSeasonAdditive());
}

std::unique_ptr<Common::RestoreSeason> Common::RestoreSeasonMultiplicativeFactory::create() const
{
    using namespace Common;
    return std::make_unique<RestoreSeasonMultiplicative>(RestoreSeasonMultiplicative());
}