//
// Created by Alberto Campi on 24/07/2019.
//

#include "Tools.h"
#include "AlgebraicExpressionInterpreter.h"
#include "../../../Global/Mappings/FactoryMappings.h"

using namespace Common;


//StringSplit class implementation
std::vector<std::string> StringSplit::split(const std::string &string, const std::string &pattern) const
{
    std::vector<std::string> rv;
    boost::split(rv, string, boost::is_any_of(pattern));
    return rv;
}


//StringSplitDecorator class hierarchy implementation
StringSplitDecorator::StringSplitDecorator() : m_strplit(), m_components() {}

void StringSplitConfigVariableDecorator::split(const std::string& string, const std::string& pattern)
{
    const unsigned int expectedConfigVariableComponents = 3; //rawConfigVariable = basename|transformationTypeCode|lag
    //const unsigned int minimumConfigVariableComponents = 1; //rawConfigVariable = basename (curve models only)
    const std::vector<std::string> rv = m_strplit.split(string, pattern);

    if (rv.size() != expectedConfigVariableComponents)
        throw std::runtime_error("E: .ConfigVariable::ConfigVariable : unexpected number of components for raw "
                                 "config variable " + string);
    else
        m_components = std::move(rv);
}

std::vector<std::string> StringSplitDecorator::get() const
{
    return m_components;
}

std::string StringSplitConfigVariableDecorator::getBasename() const
{
    return m_components[0];
}

std::string StringSplitConfigVariableDecorator::getTransformationTypeCode() const
{
    return m_components[1];
}

unsigned int StringSplitConfigVariableDecorator::getLagDependency() const
{
    const char* c_lagDependency = m_components[2].c_str();
    char* pEnd;
    const int base = 10;

    const long lagDependency = strtol(c_lagDependency, &pEnd, base);
    if (lagDependency < 0)
        throw std::runtime_error("E: StringSplitConfigVariableDecorator::getLagDependency: lag dependency must be a non-negative integer.");
    else
        return static_cast<unsigned int>(lagDependency);
}

StringSplitAlgebraicDecorator::StringSplitAlgebraicDecorator() : m_opGrammar(nullptr)
{}

StringSplitAlgebraicDecorator::StringSplitAlgebraicDecorator(const Common::OperatorsGrammar &opGrammar) : m_opGrammar(opGrammar.clone())
{}

void StringSplitAlgebraicDecorator::split(const std::string &string, const std::string &pattern)
{
    const std::string cleanString = boost::algorithm::erase_all_copy(string, " ");

    std::vector<std::string> comp, tok, op;
    std::string token;
    for (unsigned int i = 0; i < cleanString.length(); ++i)
    {
        const std::string thisToken = std::string(1,cleanString.at(i));
        if (pattern.find(thisToken) != std::string::npos)
        {
            if (!token.empty())
            {
                comp.push_back(token);
                tok.push_back(token);
            }
            tok.push_back(thisToken);
            token.clear();
            op.push_back(thisToken);
        }
        else
            token += thisToken;

        if (!token.empty() and i == cleanString.size() - 1)
        {
            comp.push_back(token);
            tok.push_back(token);
        }
    }

    m_components = std::move(comp);
    m_operators = std::move(op);
    m_tokenized = std::move(tok);
}

void StringSplitAlgebraicDecorator::splitExpression(const std::string &string)
{
    if (!m_opGrammar)
        throw std::runtime_error("StringSplitAlgebraicDecorator::split : undefined grammar for algebraic operators.");

    const std::string pattern = m_opGrammar -> getCharOperators();
    split(string, pattern);

    if (!_isValidExpression())
        throw std::runtime_error("Common::StringSplitAlgebraicDecorator::splitExpression : invalid string expression " +
        string);
}

std::vector<std::string> StringSplitAlgebraicDecorator::getOrderedOperators() const
{
    return m_operators;
}

std::vector<std::string> StringSplitAlgebraicDecorator::getTokenized() const
{
    return m_tokenized;
}

void StringSplitAlgebraicDecorator::setOperatorsGrammar(const Common::OperatorsGrammar &opGrammar)
{
    m_opGrammar = opGrammar.clone();
}

bool StringSplitAlgebraicDecorator::_isValidExpression() const
{
    if (m_opGrammar -> isOperator(m_tokenized.front()) or m_opGrammar -> isOperator(m_tokenized.back()))
        return false;

    std::vector<std::string>::const_iterator it = m_tokenized.begin();
    std::vector<std::string>::const_reverse_iterator jt = m_tokenized.rbegin();
    for (; it != m_tokenized.end() - 1; ++it)
    {
        const std::string thisToken = *it;
        const std::string nextToken = *(it + 1);
        if (m_opGrammar -> isOperator(thisToken) and m_opGrammar -> isOperator(nextToken))
            return false;
        if (m_opGrammar -> isLeftBracket(thisToken) and m_opGrammar -> isOperator(nextToken))
            return false;
        if (m_opGrammar -> isRightBracket(thisToken) and !m_opGrammar -> isOperator(nextToken))
            return false;
        if (!m_opGrammar -> isOperator(thisToken) and m_opGrammar -> isLeftBracket(nextToken))
            return false;
        if (m_opGrammar -> isOperator(thisToken) and m_opGrammar -> isRightBracket(nextToken))
            return false;
        if (m_opGrammar -> isRightBracket(thisToken) and jt == m_tokenized.rbegin())
            return false;

        if (m_opGrammar -> isLeftBracket(thisToken))
        {
            while (true)
            {
                if (m_opGrammar -> isRightBracket(*jt))
                {
                    ++jt;
                    break;
                }
                if (it >= jt.base() - 1)
                    return false;
                ++jt;
            }
        }
    }
    return true;
}


//AlgebraicExpressionParser implementation
AlgebraicExpressionParser::AlgebraicExpressionParser(const std::string &expression, const Common::OperatorsGrammar &opGrammar) :
    m_opGrammar(opGrammar.clone()), m_expr(expression)
{}

void AlgebraicExpressionParser::setExpression(const std::string &other)
{
    m_expr = other;
}

void AlgebraicExpressionParser::setOperatorGrammar(const Common::OperatorsGrammar &other)
{
    m_opGrammar = other.clone();
}

double AlgebraicExpressionParser::evaluate(const Common::AlgebraicExpressionContext &context)
{
    Common::StringSplitAlgebraicDecorator strspl(*m_opGrammar);
    strspl.splitExpression(m_expr);
    const std::vector<std::string> tokenized = strspl.getTokenized();
    m_itToken = tokenized.begin();
    m_itExprEnd = tokenized.end();

    //Using precedence climbing algorithm
    return _parseExpression(0) -> evaluate(context);
}

std::unique_ptr<Common::AlgebraicExpression> AlgebraicExpressionParser::_parsePrimary()
{
    const std::string token = *m_itToken;
    const unsigned int lowestPrecedenceValue = m_opGrammar -> lowestPriority();
    if (m_opGrammar -> isLeftBracket(token))
    {
        ++m_itToken;
        std::unique_ptr<Common::AlgebraicExpression> expr = _parseExpression(lowestPrecedenceValue);

        if (m_opGrammar -> isRightBracket(*m_itToken))
            throw std::runtime_error("Common::AlgebraicExpressionParser::_parsePrimary : parsing error. Unmatched brackets.");

        ++m_itToken;

        return expr;
    }
    else if (!m_opGrammar -> isOperator(token))
    {
        ++m_itToken;
        char* pEnd;
        const bool isNumeric = std::strtod(token.c_str(), &pEnd) != 0 or
                (std::strtod(token.c_str(), &pEnd) == 0 and *pEnd == '\0');
        if (isNumeric)
        {
            const double constant = std::strtod(token.c_str(), &pEnd);
            return std::make_unique<Common::ConstantExpression>(Common::ConstantExpression(constant));
        }
        else
            return std::make_unique<Common::VariableExpression>(Common::VariableExpression(token));
    }
    else
        throw std::runtime_error("Common::AlgebraicExpressionParser::_parsePrimary : "
                                 "parsing error. Value found where operator was expected");
}

std::unique_ptr<Common::AlgebraicExpression> AlgebraicExpressionParser::_parseExpression(unsigned int lowestPrecedenceValue)
{
    std::unique_ptr<Common::AlgebraicExpression> lhsExpr = _parsePrimary();
    while (true)
    {
        if (m_itToken == m_itExprEnd or
            !m_opGrammar -> isOperator(*m_itToken) or
            m_opGrammar -> priority(*m_itToken) < lowestPrecedenceValue)
            break;

        const std::string opSymbol = *m_itToken;
        const unsigned int opPrecedence = m_opGrammar -> priority(opSymbol);
        const unsigned int nextLowestPrecedenceValue = m_opGrammar -> isLeftAssociative(opSymbol) ? opPrecedence + 1 : opPrecedence;

        ++m_itToken;
        std::unique_ptr<Common::AlgebraicExpression> rhsExpr = _parseExpression(nextLowestPrecedenceValue);
        lhsExpr = Global::AlgebraicExpressionFactoryMapping::instance() -> getFactory(opSymbol) -> create(*lhsExpr, *rhsExpr);
    }

    return lhsExpr;
}

//Global function tools definitions
double Common::getTenorInYearsFromVariableName(const std::string& variableName)
{
    Common::StringSplit spl;
    std::string tenor = spl.split(variableName, "_").back();

    if (tenor == "ON")  //[AC] overnight rates
        return 1./365;

    const char unit = tenor.back();
    tenor.pop_back();
    const double value = std::stod(tenor);

    switch (unit)
    {
        case 'Y':
            return value;
        case 'M':
            return value/12.;
        case 'D':
            return value/365.;
        default:
            throw std::runtime_error("CurveModelDef::_getTenorInYearsFromVariableName : unknown unit code " + std::string(1, unit));
    }
}