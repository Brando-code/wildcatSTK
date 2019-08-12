//
// Created by Alberto Campi on 11/11/2018.
//

#include "LinearInterpolator.h"
#include <memory>

std::pair<double, double> Math::LinearInterpolator::interpolate(const std::map<double, double> &dataSet, double x)
{
    auto i = dataSet.begin();
    std::pair<double, double> interpolatedPoint;
    //check whether the map contains at least two data points
    if (dataSet.size() < 2)
    {
        throw std::runtime_error("E: math::LinearInterpolator::interpolate : insufficient data for interpolation.");
    }

    //handle lower extrapolation
    if (x <= i -> first)
    {
        auto next = i;
        ++next;
        const double y = i -> second + (next -> second - i -> second)/(next -> first - i -> first)*(x - i ->first);
        interpolatedPoint = std::make_pair(x, y);
        //interpolatedPoint = std::make_pair(x, i -> second);
    }
    else
    {
        //handle interpolation
        for (i = dataSet.begin(); i != dataSet.end(); ++i)
        {
            if(i -> first > x)
            {
                auto previous = i;
                --previous;
                const double x0 = previous -> first;
                const double y0 = previous -> second;
                const double x1 = i -> first;
                const double y1 = i -> second;
                const double y = y0 + (x - x0)*((y1 - y0)/(x1 - x0));
                interpolatedPoint = std::make_pair(x, y);

                break;
            }
        }
    }
    //handle upper extrapolation
    if (i == dataSet.end())
    {
        --i;
        auto previous = i;
        --previous;
        const double y = i -> second + (previous -> second - i -> second)/(previous -> first - i -> first)*(x - i ->first);
        interpolatedPoint = std::make_pair(x, y);
        //interpolatedPoint = std::make_pair(x, i -> second);
    }
    return interpolatedPoint;
}

std::map<double, double> Math::LinearInterpolator::interpolatePoints(const std::map<double, double>& dataSet,
                                                                      const std::set<double>& queryPoints)
{
    std::map<double, double> interpolatedDataSet;
    for (const auto &it : queryPoints)
        interpolatedDataSet.insert(interpolate(dataSet, it));

    return interpolatedDataSet;
}

std::unique_ptr<Math::Interpolator> Math::LinearInterpolator::clone() const
{
    return std::make_unique<Math::LinearInterpolator>(*this);
}
