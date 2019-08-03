//
// Created by Alberto Campi on 23/07/2019.
//

#ifndef WILDCATSTKCORE_TIMESERIES_H
#define WILDCATSTKCORE_TIMESERIES_H


#include <string>
#include <vector>
#include <unordered_map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "../Common.h"

class Common::TimeSeries
{
public:
    TimeSeries(std::string  variableName, const std::vector<double>& variableData,
               const std::vector<boost::gregorian::date>& dates);

    std::string getName() const;
    std::vector<double> getValues() const;
    std::vector<boost::gregorian::date> getDates() const;

    double getValue(unsigned int index) const;
    double getValue(const boost::gregorian::date& date) const;

    unsigned int getIndex(const boost::gregorian::date& date) const;

    bool operator==(const Common::TimeSeries& other) const;
    bool operator!=(const Common::TimeSeries& other) const;

private:
    std::string m_name;
    std::vector<double> m_data;
    std::vector<boost::gregorian::date> m_dates;
};


#endif //WILDCATSTKCORE_TIMESERIES_H
