//
// Created by Alberto Campi on 27/07/2019.
//

#include "DataSet.h"
#include "TimeSeries.h"


//DataSet class implementation
Common::DataSet::DataSet(const std::vector<Common::TimeSeries> &ts)
{
    for (const auto& el: ts)
    {
        m_data.emplace(el.getName(), el);
    }
}

std::unordered_map<std::string, Common::TimeSeries> Common::DataSet::getData() const
{
    return m_data;
}

void Common::DataSet::addData(const Common::TimeSeries &otherTs)
{
    //[AC] if logic gets used frequently in critical way consider storing mapping at construction stage
    const std::string key = otherTs.getName();
    if (m_data.find(key) == m_data.end())
        m_data.emplace(key, otherTs);
    else    //[AC] are we sure we want to allow this? Flagging a warning for now, it may be an error in the future.
    {
        m_data.at(key) = otherTs;
        std::cerr << "W: Common::DataSet::addData : existing data set values for variable " << key << " is being replaced." << std::endl;
    }
}

void Common::DataSet::appendValue(const std::string &variableName, const boost::gregorian::date &date, double value)
{
    m_data.at(variableName).pushBack(date, value);
}

void Common::DataSet::removeData(const std::string &variableName)
{
    m_data.erase(variableName);
}

void Common::DataSet::clearAllData()
{
    m_data.clear();
}

double Common::DataSet::getValue(const std::string &variableName, unsigned int index) const
{
    if (m_data.find(variableName) != m_data.end())
        return m_data.at(variableName).getValue(index);
    else
        throw std::runtime_error("E: DataSet::getValue : variable " + variableName + " is not in the data set.");
}

double Common::DataSet::getValue(const std::string &variableName, const boost::gregorian::date &date) const
{
    if (m_data.find(variableName) != m_data.end())
        return m_data.at(variableName).getValue(date);
    throw std::runtime_error("E: DataSet::getValue : variable " + variableName + " is not in the data set.");
}

Common::TimeSeries Common::DataSet::getTimeSeries(const std::string &variableName) const
{
    if (m_data.find(variableName) != m_data.end())
        return m_data.at(variableName);
    throw std::runtime_error("E: DataSet::getValue : variable " + variableName + " is not in the data set.");
}

bool Common::DataSet::operator==(const Common::DataSet &other) const
{
    bool rv = true;
    if (m_data.size() == other.m_data.size())
    {
        for (const auto& it: m_data)
        {
            if (other.m_data.find(it.first) != other.m_data.end() and it.second == other.m_data.at(it.first))
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

bool Common::DataSet::operator!=(const Common::DataSet &other) const
{
    return !(*this == other);
}





