//
// Created by Alberto Campi on 2019-08-10.
//

#ifndef WILDCATSTKCORE_CURVEMODELDEF_H
#define WILDCATSTKCORE_CURVEMODELDEF_H


#include <string>
#include <vector>
#include <map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <unordered_map>


namespace Common
{
    class DataSet;
    using YieldCurve = std::map<double, double>;

    class CurveModelDef
    {
    public:
        CurveModelDef(std::string curveName,
                std::string interpolationMethodName,
                const std::vector<std::string>& tenorVariableNames);

        //YieldCurve getYieldCurve(const Common::DataSet& ds, unsigned int index) const;
        YieldCurve getYieldCurve(const Common::DataSet& ds, const boost::gregorian::date& date) const;
        std::string getCurveName() const;
        std::string getInterpolationMethodName() const;

    private:
        std::unordered_map<std::string, double> m_tenors;
        std::string m_interpMethodName;
        std::string m_curveName;
    };
}


#endif //WILDCATSTKCORE_CURVEMODELDEF_H
