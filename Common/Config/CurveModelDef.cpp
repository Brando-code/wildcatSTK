//
// Created by Alberto Campi on 2019-08-10.
//

#include "CurveModelDef.h"
#include "../Types/DataSet.h"
#include "../Utils/General/Tools.h"

Common::CurveModelDef::CurveModelDef(std::string curveName, const std::vector<std::string> &tenorVariableNames) :
    m_curveName(std::move(curveName))//, m_tenorVariableNames(tenorVariableNames)
{
    for (const auto& it: tenorVariableNames)
        m_tenors.emplace(it, Common::getTenorInYearsFromVariableName(it));
}

YieldCurve Common::CurveModelDef::getYieldCurve(const Common::DataSet &ds, const boost::gregorian::date& date) const
{
    YieldCurve yc;
    for (const auto& it: m_tenors)
        yc.emplace(it.second, ds.getValue(it.first, date));

    return yc;
}

std::string Common::CurveModelDef::getCurveName() const
{
    return m_curveName;
}
