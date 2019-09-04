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
    class RestoreSeason;

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

        void set(const std::string& expression, const Common::OperatorsGrammar& grammar);

    private:
        mutable Common::AlgebraicExpressionParser m_parser;
    };

    class FormulaVariableFunctionalDeSeason : public FormulaVariable
    {
    public:
        FormulaVariableFunctionalDeSeason(const std::string& variableName, const std::string& decompositionType, unsigned int period);
        double evaluate(const Common::DataSet& ds, const boost::gregorian::date& date) const final;
        std::vector<double> getLastSeasonalCycle(const Common::DataSet &ds) const;

        void set(const std::string& variableName, const std::string& decompositionType, unsigned int period);

    private:
        std::string m_variable;
        std::unique_ptr<Common::SeasonalDecompose> m_decompPtr;
        mutable bool m_isDecomposed;

        void _decompose(const Common::DataSet& ds) const;
    };

    class FormulaVariableFunctionalRestoreSeason : public FormulaVariable
    {
    public:
        FormulaVariableFunctionalRestoreSeason(const std::string& variableName,
                                               const std::string& decompositionType,
                                               const std::vector<double>& lastSeasonalCycle);

        double evaluate(const Common::DataSet& ds, const boost::gregorian::date& date) const final;

        void set(const std::string& variableName, const std::string& decompositionType, const std::vector<double>& lastSeasonalCycle);

    private:
        std::string m_variable;
        std::unique_ptr<Common::RestoreSeason> m_restorePtr;
        std::vector<double> m_lastSeasonalCycle;
    };

    class RestoreSeason
    {
    public:
        virtual double restore(double deSeasonedValue, double seasonalValue) const = 0;

        virtual ~RestoreSeason() = default;
    };

    class RestoreSeasonAdditive : public RestoreSeason
    {
    public:
        double restore(double deSeasonedValue, double seasonalValue) const final;

    };

    class RestoreSeasonMultiplicative : public RestoreSeason
    {
    public:
        double restore(double deSeasonedValue, double seasonalValue) const final;
    };


    // Factory classes
    class RestoreSeasonFactory
    {
    public:
        virtual std::unique_ptr<Common::RestoreSeason> create() const = 0;

        virtual ~RestoreSeasonFactory() = default;
    };

    class RestoreSeasonAdditiveFactory : public RestoreSeasonFactory
    {
    public:
        std::unique_ptr<Common::RestoreSeason> create() const final;
    };

    class RestoreSeasonMultiplicativeFactory : public RestoreSeasonFactory
    {
    public:
        std::unique_ptr<Common::RestoreSeason> create() const final;
    };

}

#endif //WILDCATSTKCORE_FORMULAVARIABLE_H
