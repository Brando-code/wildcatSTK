//
// Created by Alberto Campi on 27/07/2019.
//

#ifndef WILDCATSTKCORE_DATASET_H
#define WILDCATSTKCORE_DATASET_H

#include <string>
#include <vector>
#include <unordered_map>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace Common
{
    class TimeSeries;

    class DataSet
    {
    public:
        DataSet() = default;
        explicit DataSet(const std::vector<Common::TimeSeries> &ts);

        void addData(const Common::TimeSeries &otherTs);
        void removeData(const std::string &variableName);
        void clearAllData();

        std::unordered_map<std::string, Common::TimeSeries> getData() const;
        double getValue(const std::string &variableName, unsigned int index) const;
        double getValue(const std::string &variableName, const boost::gregorian::date &date) const;
        Common::TimeSeries getTimeSeries(const std::string &variableName) const;

        bool operator==(const Common::DataSet &other) const;
        bool operator!=(const Common::DataSet &other) const;

    private:
        std::unordered_map<std::string, Common::TimeSeries> m_data;
    };

}

#endif //WILDCATSTKCORE_DATASET_H
