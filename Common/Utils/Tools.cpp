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
        m_components = rv;
}

std::vector<std::string> StringSplitConfigVariableDecorator::get() const
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
    return static_cast<unsigned int>(strtol(c_lagDependency, &pEnd, base));
}