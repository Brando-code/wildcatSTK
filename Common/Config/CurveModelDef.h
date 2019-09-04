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


namespace Math
{
    class Interpolator;
}


namespace Common
{
    class DataSet;
    class ConfigVariable;
    using YieldCurve = std::map<double, double>;

    class CurveModelDef
    {
    public:
        CurveModelDef(std::string curveName,
                const std::string& interpolationMethodName,
                const std::vector<std::string>& tenorVariableNames);
        CurveModelDef(const CurveModelDef& other);
        CurveModelDef(CurveModelDef&& other);
        CurveModelDef& operator=(const CurveModelDef& other);
        CurveModelDef&operator=(CurveModelDef&& other);

        //YieldCurve getYieldCurve(const Common::DataSet& ds, unsigned int index) const;
        YieldCurve getYieldCurve(const Common::DataSet& ds, const boost::gregorian::date& date) const;
        std::string getCurveName() const;

        double interpolate(const Common::ConfigVariable& variable,
                           const boost::gregorian::date& date,
                           const Common::DataSet& ds) const;
        
        std::unique_ptr<Common::CurveModelDef> clone() const;

        bool operator==(const Common::CurveModelDef& other) const;
        bool operator!=(const Common::CurveModelDef& other) const;

    private:
        std::unordered_map<std::string, double> m_tenors;
        std::unique_ptr<Math::Interpolator> m_interpPtr;
        std::string m_curveName;
    };
}


#endif //WILDCATSTKCORE_CURVEMODELDEF_H
