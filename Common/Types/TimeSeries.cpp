//
// Created by Alberto Campi on 23/07/2019.
//

#include "TimeSeries.h"
#include <iostream>
#include <utility>

using namespace Common;

//TimeSeries class implementation
TimeSeries::TimeSeries(std::string variableName, const std::vector<double> &variableData,
                       const std::vector<boost::gregorian::date> &dates) :
        m_name(std::move(variableName)), m_data(variableData), m_dates(dates)
{
    if (variableData.size() != dates.size())
        throw std::runtime_error("E: TimeSeries::TimeSeries : TimeSeries object cannot be constructed due to data-date size mismatch");
}

std::string TimeSeries::getName() const
{
    return m_name;
}

std::vector<double> TimeSeries::getValues() const
{
    return m_data;
}

std::vector<boost::gregorian::date> TimeSeries::getDates() const
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
    for (unsigned long i = 0; i < m_dates.size(); ++i)
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
