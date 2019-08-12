//
// Created by Alberto Campi on 2019-08-06.
//

#include "ConfigModelSpecTable.h"

Common::ConfigModelSpecTable::ConfigModelSpecTable(const std::vector<std::unique_ptr<Common::ConfigModelSpec>> &modelSpecs)
{
    for (const auto& it: modelSpecs)
        m_configModelTable.emplace(it -> getDependentVariable().getBasename(), it -> clone());
}

Common::ConfigModelSpecTable::ConfigModelSpecTable(const Common::ConfigModelSpecTable &other)
{
    for (const auto& it: other.m_configModelTable)
        m_configModelTable.emplace(it.first, it.second -> clone());
}

Common::ConfigModelSpecTable::ConfigModelSpecTable(Common::ConfigModelSpecTable &&other) :
        m_configModelTable(std::move(other.m_configModelTable)) {}

Common::ConfigModelSpecTable& Common::ConfigModelSpecTable::operator=(const Common::ConfigModelSpecTable &other)
{
    if (&other != this)
    {
        m_configModelTable.clear();
        for (const auto& it: other.m_configModelTable)
            m_configModelTable.emplace(it.first, it.second -> clone());
    }
    return *this;
}

Common::ConfigModelSpecTable& Common::ConfigModelSpecTable::operator=(Common::ConfigModelSpecTable &&other)
{
    if (&other != this)
    {
        m_configModelTable.clear();
        m_configModelTable = std::move(other.m_configModelTable);
    }
    return *this;
}

void Common::ConfigModelSpecTable::addModelSpec(const Common::ConfigModelSpec &modelSpec)
{
    const std::string key = modelSpec.getDependentVariable().getBasename();
    if (m_configModelTable.find(key) == m_configModelTable.end())
        m_configModelTable.emplace(modelSpec.getDependentVariable().getBasename(), modelSpec.clone());
    else
    {
        std::cerr << "W: Common::ConfigModelSpecTable::addModelSpec : existing specification for model " << key << " is being replaced."
                  << std::endl;
        m_configModelTable.at(key) = modelSpec.clone();
    }
}

void Common::ConfigModelSpecTable::removeData(const std::string &variableName)
{
    m_configModelTable.erase(variableName);
}

void Common::ConfigModelSpecTable::clearAllData()
{
    m_configModelTable.clear();
}

std::unordered_map<std::string, std::unique_ptr<Common::ConfigModelSpec>> Common::ConfigModelSpecTable::getConfigModelSpecTable() const
{
    Common::ConfigModelSpecTable temp(*this);
    return std::move(temp.m_configModelTable);
}

std::unique_ptr<Common::ConfigModelSpec> Common::ConfigModelSpecTable::getConfigModelSpec(const std::string &variableName) const
{
    return m_configModelTable.at(variableName) -> clone();
}

bool Common::ConfigModelSpecTable::operator==(const Common::ConfigModelSpecTable &other) const
{
    bool rv = true;
    if (other.m_configModelTable.size() == m_configModelTable.size())
    {
        for (const auto &it: other.m_configModelTable)
        {
            if (m_configModelTable.find(it.first) != m_configModelTable.end() and *m_configModelTable.at(it.first) == *other.m_configModelTable.at(it.first))
                continue;
            else
            {
                rv = false;
                break;
            }
        }
    } 
    else
        rv = false;
    
    return rv;
}

bool Common::ConfigModelSpecTable::operator!=(const Common::ConfigModelSpecTable &other) const
{
    return !(*this == other);
}