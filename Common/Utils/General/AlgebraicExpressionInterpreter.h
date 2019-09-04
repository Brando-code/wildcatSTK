//
// Created by Alberto Campi on 2019-08-21.
//

#ifndef WILDCATSTKCORE_ALGEBRAICEXPRESSIONINTERPRETER_H
#define WILDCATSTKCORE_ALGEBRAICEXPRESSIONINTERPRETER_H

#include <string>
#include <unordered_map>

namespace Common
{
    class OperatorsGrammar
    {
    public:
        virtual unsigned int priority(const std::string& symbol) const = 0;
        virtual unsigned int lowestPriority() const = 0;
        virtual bool isLeftAssociative(const std::string& symbol) const = 0;
        virtual bool isRightAssociative(const std::string& symbol) const = 0;

        virtual bool isOperator(const std::string& symbol) const = 0;
        virtual bool isFunction(const std::string& symbol) const = 0;
        virtual bool isLeftBracket(const std::string& symbol) const = 0;
        virtual bool isRightBracket(const std::string& symbol) const = 0;

        virtual std::string getCharOperators() const = 0;

        virtual std::unique_ptr<Common::OperatorsGrammar> clone() const = 0;

        virtual ~OperatorsGrammar() = default;
    };

    class AlgebraicOperatorsGrammar : public OperatorsGrammar
    {
    public:
        AlgebraicOperatorsGrammar();

        unsigned int priority(const std::string& symbol) const final;
        unsigned int lowestPriority() const final;
        bool isLeftAssociative(const std::string& symbol) const final;
        bool isRightAssociative(const std::string& symbol) const final;

        bool isOperator(const std::string& symbol) const final;
        bool isFunction(const std::string& symbol) const final;
        bool isLeftBracket(const std::string& symbol) const final;
        bool isRightBracket(const std::string& symbol) const final;

        std::string getCharOperators() const final;

        std::unique_ptr<Common::OperatorsGrammar> clone() const final;

    private:
        std::unordered_map<std::string, unsigned int> m_priorityTable;
        std::unordered_map<std::string, std::string> m_associativityTable;
        const unsigned int m_lowestPriority;
    };


    class AlgebraicExpressionContext
    {
    public:
        AlgebraicExpressionContext() = default;
        AlgebraicExpressionContext(const AlgebraicExpressionContext& other) = default;
        explicit AlgebraicExpressionContext(const std::unordered_map<std::string, double>& keyValues);

        void setValue(const std::string& key, double value);
        double getValue(const std::string& key) const;

    private:
        std::unordered_map<std::string, double> m_keyValues;
    };


    class AlgebraicExpression
    {
    public:
        virtual double evaluate(const AlgebraicExpressionContext& context) const = 0;
        virtual std::unique_ptr<Common::AlgebraicExpression> clone() const = 0;

        virtual ~AlgebraicExpression() = default;
    };

    class AdditionExpression : public AlgebraicExpression
    {
    public:
        AdditionExpression(const AlgebraicExpression& lhs, const AlgebraicExpression& rhs);
        AdditionExpression(const AdditionExpression& other);

        double evaluate(const AlgebraicExpressionContext& context) const final;
        std::unique_ptr<Common::AlgebraicExpression> clone() const final;

    private:
        std::unique_ptr<Common::AlgebraicExpression> m_lhsExprPtr, m_rhsExprPtr;
    };

    class SubstractionExpression : public AlgebraicExpression
    {
    public:
        SubstractionExpression(const AlgebraicExpression& lhs, const AlgebraicExpression& rhs);
        SubstractionExpression(const SubstractionExpression& other);

        double evaluate(const AlgebraicExpressionContext& context) const final;
        std::unique_ptr<Common::AlgebraicExpression> clone() const final;

    private:
        std::unique_ptr<Common::AlgebraicExpression> m_lhsExprPtr, m_rhsExprPtr;
    };

    class MultiplicationExpression : public AlgebraicExpression
    {
    public:
        MultiplicationExpression(const AlgebraicExpression& lhs, const AlgebraicExpression& rhs);
        MultiplicationExpression(const MultiplicationExpression& other);

        double evaluate(const AlgebraicExpressionContext& context) const final;
        std::unique_ptr<Common::AlgebraicExpression> clone() const final;

    private:
        std::unique_ptr<Common::AlgebraicExpression> m_lhsExprPtr, m_rhsExprPtr;
    };

    class DivisionExpression : public AlgebraicExpression
    {
    public:
        DivisionExpression(const AlgebraicExpression& lhs, const AlgebraicExpression& rhs);
        DivisionExpression(const DivisionExpression& other);

        double evaluate(const AlgebraicExpressionContext& context) const final;
        std::unique_ptr<Common::AlgebraicExpression> clone() const final;

    private:
        std::unique_ptr<Common::AlgebraicExpression> m_lhsExprPtr, m_rhsExprPtr;
    };

    class ExponentiationExpression : public AlgebraicExpression
    {
    public:
        ExponentiationExpression(const AlgebraicExpression& base, const AlgebraicExpression& exponent);
        ExponentiationExpression(const ExponentiationExpression& other);

        double evaluate(const AlgebraicExpressionContext& context) const final;
        std::unique_ptr<Common::AlgebraicExpression> clone() const final;

    private:
        std::unique_ptr<Common::AlgebraicExpression> m_baseExprPtr, m_exponentExprPtr;
    };

    class VariableExpression : public AlgebraicExpression
    {
    public:
        VariableExpression(const std::string& variable);

        double evaluate(const AlgebraicExpressionContext& context) const final;
        std::unique_ptr<Common::AlgebraicExpression> clone() const final;
        std::string getVariable() const;

    private:
        std::string m_variableName;
    };

    class ConstantExpression : public AlgebraicExpression
    {
    public:
        ConstantExpression(double constant);

        double evaluate(const AlgebraicExpressionContext& context) const final;
        std::unique_ptr<Common::AlgebraicExpression> clone() const final;
        double getConstant() const;

    private:
        double m_constant;
    };


    //Operators Factory Class Hierarchy
    class BinaryOperatorExpressionFactory
    {
    public:
        virtual std::unique_ptr<Common::AlgebraicExpression> create(const Common::AlgebraicExpression& lhs,
                const Common::AlgebraicExpression& rhs) const = 0;

        virtual ~BinaryOperatorExpressionFactory() = default;
    };

    class AdditionExpressionFactory : public BinaryOperatorExpressionFactory
    {
    public:
        std::unique_ptr<Common::AlgebraicExpression> create(const Common::AlgebraicExpression& lhs,
                                                            const Common::AlgebraicExpression& rhs) const final;
    };

    class SubstractionExpressionFactory : public BinaryOperatorExpressionFactory
    {
    public:
        std::unique_ptr<Common::AlgebraicExpression> create(const Common::AlgebraicExpression& lhs,
                                                            const Common::AlgebraicExpression& rhs) const final;
    };

    class MultiplicationExpressionFactory : public BinaryOperatorExpressionFactory
    {
    public:
        std::unique_ptr<Common::AlgebraicExpression> create(const Common::AlgebraicExpression& lhs,
                                                            const Common::AlgebraicExpression& rhs) const final;
    };

    class DivisionExpressionFactory : public BinaryOperatorExpressionFactory
    {
    public:
        std::unique_ptr<Common::AlgebraicExpression> create(const Common::AlgebraicExpression& lhs,
                                                            const Common::AlgebraicExpression& rhs) const final;
    };

    class ExponentiationExpressionFactory : public BinaryOperatorExpressionFactory
    {
    public:
        std::unique_ptr<Common::AlgebraicExpression> create(const Common::AlgebraicExpression& lhs,
                                                            const Common::AlgebraicExpression& rhs) const final;
    };
}


#endif //WILDCATSTKCORE_ALGEBRAICEXPRESSIONINTERPRETER_H
