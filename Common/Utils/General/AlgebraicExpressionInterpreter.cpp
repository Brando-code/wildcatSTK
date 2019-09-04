//
// Created by Alberto Campi on 2019-08-21.
//

#include <cmath>
#include "AlgebraicExpressionInterpreter.h"
#include "../../../Global/Symbols/AlgebraicOperatorSymbols.h"

//AlgebraicOperatorsGrammar class implementation
Common::AlgebraicOperatorsGrammar::AlgebraicOperatorsGrammar() : m_lowestPriority(1)
{
    Global::AlgebraicOperatorSymbols* ptr = Global::AlgebraicOperatorSymbols::instance();

    //Construct operator priority table
    m_priorityTable.emplace(ptr -> getExponentiationSymbol(), 3);
    m_priorityTable.emplace(ptr -> getMultiplicationSymbol(), 2);
    m_priorityTable.emplace(ptr -> getDivisionSymbol(), 2);
    m_priorityTable.emplace(ptr -> getAdditionSymbol(), 1);
    m_priorityTable.emplace(ptr -> getSubstractionSymbol(), 1);
    //m_priorityTable.emplace(ptr -> getLeftBracketSymbol(), 0);
    //m_priorityTable.emplace(ptr -> getRightBracketSymbol(), 0);

    //Construct associativity table
    const std::string left = "l", right = "r";
    m_associativityTable.emplace(ptr -> getExponentiationSymbol(), right);
    m_associativityTable.emplace(ptr -> getMultiplicationSymbol(), left);
    m_associativityTable.emplace(ptr -> getDivisionSymbol(), left);
    m_associativityTable.emplace(ptr -> getAdditionSymbol(), left);
    m_associativityTable.emplace(ptr -> getSubstractionSymbol(), left);
}

unsigned int Common::AlgebraicOperatorsGrammar::priority(const std::string &symbol) const
{
    return m_priorityTable.at(symbol);
}

unsigned int Common::AlgebraicOperatorsGrammar::lowestPriority() const
{
    return m_lowestPriority;
}

bool Common::AlgebraicOperatorsGrammar::isLeftAssociative(const std::string &symbol) const
{
    const std::string left = "l";
    return m_associativityTable.at(symbol) == left;
}

bool Common::AlgebraicOperatorsGrammar::isRightAssociative(const std::string &symbol) const
{
    const std::string right = "r";
    return m_associativityTable.at(symbol) == right;
}

bool Common::AlgebraicOperatorsGrammar::isOperator(const std::string &symbol) const
{
    Global::AlgebraicOperatorSymbols* ptr = Global::AlgebraicOperatorSymbols::instance();
    return symbol == ptr -> getAdditionSymbol() or
    symbol == ptr -> getSubstractionSymbol() or
    symbol == ptr -> getMultiplicationSymbol() or
    symbol == ptr -> getDivisionSymbol() or
    symbol == ptr -> getExponentiationSymbol();
}

bool Common::AlgebraicOperatorsGrammar::isFunction(const std::string &symbol) const
{
    //Currently functions are not supported
    return false;
}

bool Common::AlgebraicOperatorsGrammar::isLeftBracket(const std::string &symbol) const
{
    Global::AlgebraicOperatorSymbols* ptr = Global::AlgebraicOperatorSymbols::instance();
    return symbol == ptr -> getLeftBracketSymbol();
}

bool Common::AlgebraicOperatorsGrammar::isRightBracket(const std::string &symbol) const
{
    Global::AlgebraicOperatorSymbols* ptr = Global::AlgebraicOperatorSymbols::instance();
    return symbol == ptr -> getRightBracketSymbol();
}

std::string Common::AlgebraicOperatorsGrammar::getCharOperators() const
{
    Global::AlgebraicOperatorSymbols* ptr = Global::AlgebraicOperatorSymbols::instance();
    return ptr -> getAdditionSymbol() +
    ptr -> getSubstractionSymbol() +
    ptr -> getMultiplicationSymbol() +
    ptr -> getDivisionSymbol() +
    ptr -> getExponentiationSymbol() +
    ptr -> getLeftBracketSymbol() +
    ptr -> getRightBracketSymbol();
}

std::unique_ptr<Common::OperatorsGrammar> Common::AlgebraicOperatorsGrammar::clone() const
{
    return std::make_unique<Common::AlgebraicOperatorsGrammar>(*this);
}


//AlgebraicExpressionContext class implementation
Common::AlgebraicExpressionContext::AlgebraicExpressionContext(const std::unordered_map<std::string, double> &keyValues) :
    m_keyValues(keyValues)
{}

double Common::AlgebraicExpressionContext::getValue(const std::string &key) const
{
    return m_keyValues.at(key);
}

void Common::AlgebraicExpressionContext::setValue(const std::string &key, double value)
{
    m_keyValues.at(key) = value;
}


//Abstact Expression Tree classes implementation
Common::AdditionExpression::AdditionExpression(const Common::AlgebraicExpression &lhs, const Common::AlgebraicExpression &rhs) :
    m_lhsExprPtr(lhs.clone()), m_rhsExprPtr(rhs.clone())
{}

Common::AdditionExpression::AdditionExpression(const Common::AdditionExpression &other) :
    m_lhsExprPtr(other.m_lhsExprPtr -> clone()), m_rhsExprPtr(other.m_rhsExprPtr -> clone())
{}

double Common::AdditionExpression::evaluate(const Common::AlgebraicExpressionContext &context) const
{
    return m_lhsExprPtr -> evaluate(context) + m_rhsExprPtr -> evaluate(context);
}

std::unique_ptr<Common::AlgebraicExpression> Common::AdditionExpression::clone() const
{
    return std::make_unique<Common::AdditionExpression>(*this);
}

Common::SubstractionExpression::SubstractionExpression(const Common::AlgebraicExpression &lhs, const Common::AlgebraicExpression &rhs) :
        m_lhsExprPtr(lhs.clone()), m_rhsExprPtr(rhs.clone())
{}

Common::SubstractionExpression::SubstractionExpression(const Common::SubstractionExpression &other) :
        m_lhsExprPtr(other.m_lhsExprPtr -> clone()), m_rhsExprPtr(other.m_rhsExprPtr -> clone())
{}

double Common::SubstractionExpression::evaluate(const Common::AlgebraicExpressionContext &context) const
{
    return m_lhsExprPtr -> evaluate(context) - m_rhsExprPtr -> evaluate(context);
}

std::unique_ptr<Common::AlgebraicExpression> Common::SubstractionExpression::clone() const
{
    return std::make_unique<Common::SubstractionExpression>(*this);
}

Common::MultiplicationExpression::MultiplicationExpression(const Common::AlgebraicExpression &lhs, const Common::AlgebraicExpression &rhs) :
        m_lhsExprPtr(lhs.clone()), m_rhsExprPtr(rhs.clone())
{}

Common::MultiplicationExpression::MultiplicationExpression(const Common::MultiplicationExpression &other) :
        m_lhsExprPtr(other.m_lhsExprPtr -> clone()), m_rhsExprPtr(other.m_rhsExprPtr -> clone())
{}

double Common::MultiplicationExpression::evaluate(const Common::AlgebraicExpressionContext &context) const
{
    return m_lhsExprPtr -> evaluate(context) * m_rhsExprPtr -> evaluate(context);
}

std::unique_ptr<Common::AlgebraicExpression> Common::MultiplicationExpression::clone() const
{
    return std::make_unique<Common::MultiplicationExpression>(*this);
}

Common::DivisionExpression::DivisionExpression(const Common::AlgebraicExpression &lhs, const Common::AlgebraicExpression &rhs) :
        m_lhsExprPtr(lhs.clone()), m_rhsExprPtr(rhs.clone())
{}

Common::DivisionExpression::DivisionExpression(const Common::DivisionExpression &other) :
        m_lhsExprPtr(other.m_lhsExprPtr -> clone()), m_rhsExprPtr(other.m_rhsExprPtr -> clone())
{}

double Common::DivisionExpression::evaluate(const Common::AlgebraicExpressionContext &context) const
{
    return m_lhsExprPtr -> evaluate(context) / m_rhsExprPtr -> evaluate(context);
}

std::unique_ptr<Common::AlgebraicExpression> Common::DivisionExpression::clone() const
{
    return std::make_unique<Common::DivisionExpression>(*this);
}

Common::ExponentiationExpression::ExponentiationExpression(const Common::AlgebraicExpression &base, const Common::AlgebraicExpression &exponent) :
        m_baseExprPtr(base.clone()), m_exponentExprPtr(exponent.clone())
{}

Common::ExponentiationExpression::ExponentiationExpression(const Common::ExponentiationExpression &other) :
        m_baseExprPtr(other.m_baseExprPtr -> clone()), m_exponentExprPtr(other.m_exponentExprPtr -> clone())
{}

double Common::ExponentiationExpression::evaluate(const Common::AlgebraicExpressionContext &context) const
{
    return pow(m_baseExprPtr -> evaluate(context), m_exponentExprPtr -> evaluate(context));
}

std::unique_ptr<Common::AlgebraicExpression> Common::ExponentiationExpression::clone() const
{
    return std::make_unique<Common::ExponentiationExpression>(*this);
}

Common::VariableExpression::VariableExpression(const std::string &variable) : m_variableName(variable)
{}

double Common::VariableExpression::evaluate(const Common::AlgebraicExpressionContext &context) const
{
    return context.getValue(m_variableName);
}

std::string Common::VariableExpression::getVariable() const
{
    return m_variableName;
}

std::unique_ptr<Common::AlgebraicExpression> Common::VariableExpression::clone() const
{
    return std::make_unique<Common::VariableExpression>(*this);
}

Common::ConstantExpression::ConstantExpression(double constant) : m_constant(constant)
{}

double Common::ConstantExpression::evaluate(const Common::AlgebraicExpressionContext &context) const
{
    return getConstant();
}

double Common::ConstantExpression::getConstant() const
{
    return m_constant;
}

std::unique_ptr<Common::AlgebraicExpression> Common::ConstantExpression::clone() const
{
    return std::make_unique<Common::ConstantExpression>(*this);
}


//Abstract factory classes implementation
std::unique_ptr<Common::AlgebraicExpression> Common::AdditionExpressionFactory::create(const Common::AlgebraicExpression &lhs,
                                                                                       const Common::AlgebraicExpression &rhs) const
{
    return std::make_unique<Common::AdditionExpression>(Common::AdditionExpression(lhs, rhs));
}

std::unique_ptr<Common::AlgebraicExpression> Common::SubstractionExpressionFactory::create(const Common::AlgebraicExpression &lhs,
                                                                                           const Common::AlgebraicExpression &rhs) const
{
    return std::make_unique<Common::SubstractionExpression>(Common::SubstractionExpression(lhs, rhs));
}

std::unique_ptr<Common::AlgebraicExpression> Common::MultiplicationExpressionFactory::create(const Common::AlgebraicExpression &lhs,
                                                                                             const Common::AlgebraicExpression &rhs) const
{
    return std::make_unique<Common::MultiplicationExpression>(Common::MultiplicationExpression(lhs, rhs));
}

std::unique_ptr<Common::AlgebraicExpression> Common::DivisionExpressionFactory::create(const Common::AlgebraicExpression &lhs,
                                                                                       const Common::AlgebraicExpression &rhs) const
{
    return std::make_unique<Common::DivisionExpression>(Common::DivisionExpression(lhs, rhs));
}

std::unique_ptr<Common::AlgebraicExpression> Common::ExponentiationExpressionFactory::create(const Common::AlgebraicExpression &lhs,
                                                                                             const Common::AlgebraicExpression &rhs) const
{
    return std::make_unique<Common::ExponentiationExpression>(Common::ExponentiationExpression(lhs, rhs));
}




