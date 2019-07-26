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
    TimeSeries(const std::string& variableName, const std::vector<double>& variableData,
               const std::vector<boost::gregorian::date>& dates);

    const std::string getName() const;
    const std::vector<double> getValues() const;
    const std::vector<boost::gregorian::date> getDates() const;

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


class Common::DataSet
{
public:
    DataSet() = default;
    explicit DataSet(const std::vector<Common::TimeSeries>& ts);

    void addData(const Common::TimeSeries& otherTs);
    void removeData(const std::string& variableName);
    const std::unordered_map<std::string, Common::TimeSeries> getData() const;

    double getValue(const std::string& variableName, unsigned int index) const;
    double getValue(const std::string& variableName, const boost::gregorian::date& date) const;

private:
    std::unordered_map<std::string, Common::TimeSeries> m_data;
};

#endif //WILDCATSTKCORE_TIMESERIES_H
