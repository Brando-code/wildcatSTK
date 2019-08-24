//
// Created by Alberto Campi on 2019-08-22.
//

#include "AlgebraicOperatorSymbols.h"

Global::AlgebraicOperatorSymbols* Global::AlgebraicOperatorSymbols::m_instance = nullptr;

Global::AlgebraicOperatorSymbols::AlgebraicOperatorSymbols() :
    m_additionSymbol("+"),
    m_substractionSymbol("-"),
    m_multiplicationSymbol("*"),
    m_divisionSymbol("/"),
    m_exponentiationSymbol("^"),
    m_leftBracketSymbol("("),
    m_rightBracketSymbol(")")
{}

Global::AlgebraicOperatorSymbols* Global::AlgebraicOperatorSymbols::instance()
{
    if (!m_instance)
        m_instance = new AlgebraicOperatorSymbols();

    return m_instance;
}

std::string Global::AlgebraicOperatorSymbols::getAdditionSymbol() const
{
    return m_additionSymbol;
}

std::string Global::AlgebraicOperatorSymbols::getSubstractionSymbol() const
{
    return m_substractionSymbol;
}

std::string Global::AlgebraicOperatorSymbols::getMultiplicationSymbol() const
{
    return m_multiplicationSymbol;
}

std::string Global::AlgebraicOperatorSymbols::getDivisionSymbol() const
{
    return m_divisionSymbol;
}

std::string Global::AlgebraicOperatorSymbols::getExponentiationSymbol() const
{
    return m_exponentiationSymbol;
}

std::string Global::AlgebraicOperatorSymbols::getLeftBracketSymbol() const
{
    return m_leftBracketSymbol;
}

std::string Global::AlgebraicOperatorSymbols::getRightBracketSymbol() const
{
    return m_rightBracketSymbol;
}

