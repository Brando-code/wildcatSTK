//
// Created by Alberto Campi on 23/07/2019.
//

#include "TimeSeries.h"
#include <iostream>

using namespace Common;

//TimeSeries class implementation
TimeSeries::TimeSeries(const std::string &variableName, const std::vector<double> &variableData,
                       const std::vector<boost::gregorian::date> &dates) :
        m_name(variableName), m_data(variableData), m_dates(dates)
{
    if (variableData.size() != dates.size())
        throw std::runtime_error("E: TimeSeries::TimeSeries : TimeSeries object cannot be constructed due to data-date size mismatch");
}

const std::string TimeSeries::getName() const
{
    return m_name;
}

const std::vector<double> TimeSeries::getValues() const
{
    return m_data;
}

const std::vector<boost::gregorian::date> TimeSeries::getDates() const
{
    return m_dates;
}

double TimeSeries::getValue(unsigned int index) const
{
    if (index < m_data.size())
        return m_data.at(index);
    else
        throw std::out_of_range("E: TimeSeries::getValue : index " + std::to_string(index) + " is not in range.");
}

double TimeSeries::getValue(const boost::gregorian::date& date) const
{
    return m_data.at(getIndex(date));
}

unsigned int TimeSeries::getIndex(const boost::gregorian::date& date) const
{
    for (unsigned int i = 0; i < m_dates.size(); ++i)
    {
        if (m_dates[i] == date)
            return i;
    }
    throw std::out_of_range("E: TimeSeries::getValue : date " + boost::gregorian::to_simple_string(date) + "is not in range.");
}

bool TimeSeries::operator==(const Common::TimeSeries &other) const
{
    return m_name == other.m_name and m_data == other.m_data and m_dates == other.m_dates;
}

bool TimeSeries::operator!=(const Common::TimeSeries &other) const
{
    return !(*this == other);
}


//DataSet class implementation
DataSet::DataSet(const std::vector<TimeSeries> &ts)
{
    for (const auto& el: ts)
    {
        m_data.emplace(el.getName(), el);
    }
}

const std::unordered_map<std::string, TimeSeries> DataSet::getData() const
{
    return m_data;
}

void DataSet::addData(const TimeSeries &otherTs)
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

void DataSet::removeData(const std::string &variableName)
{
    m_data.erase(variableName);
}

double DataSet::getValue(const std::string &variableName, unsigned int index) const
{
    if (m_data.find(variableName) != m_data.end())
        return m_data.at(variableName).getValue(index);
    else
        throw std::runtime_error("E: DataSet::getValue : variable " + variableName + " is not in the data set.");
}

double DataSet::getValue(const std::string &variableName, const boost::gregorian::date &date) const
{
    if (m_data.find(variableName) != m_data.end())
        return m_data.at(variableName).getValue(date);
    throw std::runtime_error("E: DataSet::getValue : variable " + variableName + " is not in the data set.");
}