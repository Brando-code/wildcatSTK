//
// Created by Alberto Campi on 23/07/2019.
//

#ifndef WILDCATSTKCORE_TIMESERIES_H
#define WILDCATSTKCORE_TIMESERIES_H


#include <string>
#include <vector>
#include "../Common.h"

class Common::TimeSeries
{
public:
    TimeSeries(const std::string& variableName, const std::vector<double>& variableData);

    std::string getName() const;
    std::vector<double> getValues() const;

private:
    std::pair<std::string, std::vector<double>> m_data;

};


#endif //WILDCATSTKCORE_TIMESERIES_H
