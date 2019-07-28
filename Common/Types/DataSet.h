//
// Created by Alberto Campi on 27/07/2019.
//

#ifndef WILDCATSTKCORE_DATASET_H
#define WILDCATSTKCORE_DATASET_H

#include <string>
#include <vector>
#include <unordered_map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "TimeSeries.h"
#include "../Common.h"

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

    Common::TimeSeries getTimeSeries(const std::string& variableName) const;

private:
    std::unordered_map<std::string, Common::TimeSeries> m_data;
};


#endif //WILDCATSTKCORE_DATASET_H
