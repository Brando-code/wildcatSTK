//
// Created by Alberto Campi on 23/07/2019.
//

#include "TimeSeries.h"

using namespace Common;

TimeSeries::TimeSeries(const std::string& variableName, const std::vector<double>& variableData) :
        m_data(std::make_pair(variableName, variableData)) {}

std::string TimeSeries::getName() const
{
    return m_data.first;
}

std::vector<double > TimeSeries::getValues() const
{
    return m_data.second;
}