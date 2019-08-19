//
// Created by Alberto Campi on 24/07/2019.
//

#include "Tools.h"

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

void StringSplitAlgebraicDecorator::split(const std::string &string, const std::string &pattern)
{
    const std::string cleanString = boost::algorithm::erase_all_copy(string, " ");
    m_components = m_strplit.split(cleanString, pattern);

    std::vector<std::string> op;
    for (unsigned int i = 0; i < cleanString.length(); ++i)
        if(StringSplitAlgebraicDecorator::_isOperator(std::string(1,cleanString.at(i))))
            op.push_back(std::string(1, cleanString.at(i)));

    m_operators = std::move(op);
}

void StringSplitAlgebraicDecorator::splitExpression(const std::string &string)
{
    const std::string pattern = "^*/+-";
    split(string, pattern);

    if (!_isValidExpression())
        throw std::runtime_error("Common::StringSplitAlgebraicDecorator::splitExpression : invalid string expression " +
        string);
}

std::vector<std::string> StringSplitAlgebraicDecorator::getOrderedOperators() const
{
    return m_operators;
}

bool StringSplitAlgebraicDecorator::_isOperator(const std::string &string)
{
    return string == "^" or string == "*" or string == "/" or string == "+" or string == "-";
}

bool StringSplitAlgebraicDecorator::_isValidExpression() const
{
    for (const auto& it: m_components)
        if (it.empty())
            return false;

    return true;
}


//Algebraic expression parser implementation
Common::AlgebraicExpressionParser::AlgebraicExpressionParser(const std::string &expression)
{
    m_strsplit.splitExpression(expression);
}

void Common::AlgebraicExpressionParser::setExpression(const std::string &expression)
{
    m_strsplit.splitExpression(expression);
}

std::vector<std::string> Common::AlgebraicExpressionParser::getComponents() const
{
    return m_strsplit.get();
}

double Common::AlgebraicExpressionParser::evaluate(const std::unordered_map<std::string, double> variableValuePairs) const
{
    int i = 0;
    std::vector<double> runningComponents;
    std::vector<std::string> runningOperators;
    while(i < m_strsplit.get().size())
    {
        char* pEnd;
        const double component = std::strtod(m_strsplit.get().at(i).c_str(), &pEnd) == 0 ?
                                 variableValuePairs.at(m_strsplit.get().at(i)) : std::strtod(m_strsplit.get().at(i).c_str(), &pEnd);//variableValuePairs.at(m_strsplit.get().at(i));
        if (i < m_strsplit.getOrderedOperators().size() and m_strsplit.getOrderedOperators().at(i) == "^")
        {
            const double base = component;
            double exponent = 1;

            while (i < m_strsplit.getOrderedOperators().size() and m_strsplit.getOrderedOperators().at(i) == "^")
            {
                exponent *= variableValuePairs.at(m_strsplit.get().at(i + 1));
                ++i;
            }
            runningComponents.push_back(pow(base, exponent));
        }
        else
            runningComponents.push_back(component);

        if (i < m_strsplit.getOrderedOperators().size())
            runningOperators.push_back(m_strsplit.getOrderedOperators().at(i));
        ++i;
    }

    i = 0;
    std::vector<double> _runningComponents;
    std::vector<std::string> _runningOperators;
    while(i < runningComponents.size())
    {
        const double component = runningComponents.at(i);
        if (i < runningOperators.size() and (runningOperators.at(i) == "*" or runningOperators.at(i) == "/"))
        {
            double factor = component;

            while (i < runningOperators.size() and (runningOperators.at(i) == "*" or runningOperators.at(i) == "/"))
            {
                if (runningOperators.at(i) == "*")
                    factor *= runningComponents.at(i + 1);
                if (runningOperators.at(i) == "/")
                    factor /= runningComponents.at(i + 1);
                ++i;
            }
            _runningComponents.push_back(factor);
        }
        else
            _runningComponents.push_back(component);

        if (i < runningOperators.size())
            _runningOperators.push_back(runningOperators.at(i));
        ++i;
    }

    i = 0;
    runningComponents = _runningComponents;
    runningOperators = _runningOperators;
    double value = runningComponents.at(i);
    while(i < runningOperators.size())
    {
        if (runningOperators.at(i) == "+")
            value += runningComponents.at(i + 1);
        if (runningOperators.at(i) == "-")
            value -= runningComponents.at(i + 1);
        ++i;
    }
    return value;
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