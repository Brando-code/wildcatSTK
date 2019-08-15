//
// Created by Alberto Campi on 2019-08-10.
//

#include "CurveModelDef.h"
#include "../Types/DataSet.h"
#include "../Utils/General/Tools.h"

Common::CurveModelDef::CurveModelDef(std::string curveName, std::string interpolationMethodName,
                                    const std::vector<std::string> &tenorVariableNames) :
    m_curveName(std::move(curveName)), m_interpMethodName(std::move(interpolationMethodName))
{
    for (const auto& it: tenorVariableNames)
        m_tenors.emplace(it, Common::getTenorInYearsFromVariableName(it));
}

Common::YieldCurve Common::CurveModelDef::getYieldCurve(const Common::DataSet &ds, const boost::gregorian::date& date) const
{
    YieldCurve yc;
    for (const auto& it: m_tenors)
        yc.emplace(it.second, ds.getValue(it.first, date));

    return std::move(yc);
}

std::string Common::CurveModelDef::getCurveName() const
{
    return m_curveName;
}

std::string Common::CurveModelDef::getInterpolationMethodName() const
{
    return m_interpMethodName;
}
