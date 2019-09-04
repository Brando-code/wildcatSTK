//
// Created by Alberto Campi on 2019-08-10.
//

#include "CurveModelDef.h"
#include "ConfigVariable.h"
#include "../Types/DataSet.h"
#include "../Utils/General/Tools.h"
#include "../../Global/Mappings/FactoryMappings.h"
#include "../Math/Interpolation/Interpolator.h"


Common::CurveModelDef::CurveModelDef(std::string curveName,
                                     const std::string& interpolationMethodName,
                                     const std::vector<std::string> &tenorVariableNames) :
    m_curveName(std::move(curveName)),
    m_interpPtr(Global::InterpolatorFactoryMapping::instance() -> getFactory(interpolationMethodName) -> create())
{
    for (const auto& it: tenorVariableNames)
        m_tenors.emplace(it, Common::getTenorInYearsFromVariableName(it));
}

Common::CurveModelDef::CurveModelDef(const Common::CurveModelDef &other) :
    m_curveName(other.m_curveName),
    m_tenors(other.m_tenors),
    m_interpPtr(other.m_interpPtr -> clone()) {}

Common::CurveModelDef::CurveModelDef(Common::CurveModelDef &&other) :
        m_curveName(other.m_curveName),
        m_tenors(other.m_tenors),
        m_interpPtr(std::move(other.m_interpPtr)) {}

Common::CurveModelDef& Common::CurveModelDef::operator=(const Common::CurveModelDef &other)
{
    if (&other != this)
    {
        m_curveName = other.m_curveName;
        m_tenors = other.m_tenors;
        m_interpPtr = other.m_interpPtr -> clone();
    }

    return *this;
}

Common::CurveModelDef& Common::CurveModelDef::operator=(Common::CurveModelDef &&other)
{
    if (&other != this)
    {
        m_curveName = other.m_curveName;
        m_tenors = other.m_tenors;
        m_interpPtr = std::move(other.m_interpPtr);
    }

    return *this;
}

Common::YieldCurve Common::CurveModelDef::getYieldCurve(const Common::DataSet &ds, const boost::gregorian::date& date) const
{
    YieldCurve r;
    for (const auto& it: m_tenors)
        r.emplace(it.second, ds.getValue(it.first, date));

    return std::move(r);
}

std::string Common::CurveModelDef::getCurveName() const
{
    return m_curveName;
}

double Common::CurveModelDef::interpolate(const Common::ConfigVariable &variable, const boost::gregorian::date &date,
                                          const Common::DataSet &ds) const
{
    const Common::YieldCurve r = getYieldCurve(ds, date);
    const double t = getTenorInYearsFromVariableName(variable.getBasename());
    return m_interpPtr -> interpolate(r, t).second;
}

std::unique_ptr<Common::CurveModelDef> Common::CurveModelDef::clone() const
{
    return std::make_unique<Common::CurveModelDef>(*this);
}

bool Common::CurveModelDef::operator==(const Common::CurveModelDef &other) const
{
    return m_curveName == other.m_curveName and
           m_tenors == other.m_tenors and
           typeid(m_interpPtr).name() == typeid(other.m_interpPtr).name();
}

bool Common::CurveModelDef::operator!=(const Common::CurveModelDef &other) const
{
    return !(*this == other);
}