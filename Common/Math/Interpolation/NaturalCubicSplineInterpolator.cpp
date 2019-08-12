//
// Created by Alberto Campi on 10/12/2018.
//

#include "NaturalCubicSplineInterpolator.h"
#include <iostream>
#include <vector>

void Math::NaturalCubicSplineInterpolator::fitSpline(const std::map<double, double> &dataSet)
{
    if (dataSet.size() < 2)
    {
        throw std::runtime_error("E: math::NaturalCubicSplineInterpolator::fitSpline : insufficient data for interpolation.");
    }

    if (m_dataSet == dataSet)
    {
        return;
    }

    //If a new data set if being passed in, fit spline with natural boundary conditions
    m_splineFunction.clear();
    m_dataSet = dataSet;

    const long N = dataSet.size();
    std::vector<double> alpha;

    for (auto it = ++dataSet.begin(); it != --dataSet.end(); ++it)
    {
        auto previous = it, next = it;
        --previous, ++next;
        const double value = 3*(next -> second - it -> second)/(next -> first - it -> first) -
                3*(it -> second - previous -> second)/(it -> first - previous -> first);
        alpha.push_back(value);
    }

    std::vector<double> mu, z;
    mu.push_back(0), z.push_back(0);
    long i = 1;

    for (auto it = ++dataSet.begin(); it != --dataSet.end(); ++it)
    {
        auto previous = it, next = it;
        --previous, ++next;
        const double l = 2*(next -> first - previous -> first) - (it -> first - previous -> first)*mu[i-1];
        mu.push_back((next -> first - it -> first)/l);
        z.push_back((alpha[i-1] - (it -> first - previous -> first)*z[i-1])/l);
        ++i;
    }

    CSCoeffs coeffs{};
    double x = dataSet.rbegin() -> first;
    coeffs.a = dataSet.rbegin() -> second, coeffs.b = 0, coeffs.c = 0, coeffs.d = 0;
    m_splineFunction.insert(std::make_pair(x, coeffs));
    i = N-2;

    for (auto it = ++dataSet.rbegin(); it != dataSet.rend(); ++it)
    {
        auto next = it.base();
        //--next;
        const double h = next -> first - it -> first;
        x = it -> first;
        coeffs.a = it -> second;
        coeffs.c = z[i] - mu[i]*m_splineFunction.at(next -> first).c;
        coeffs.b = (m_splineFunction.at(next -> first).a - coeffs.a)/h - h*(m_splineFunction.at(next -> first).c + 2*coeffs.c)/3;
        coeffs.d = (m_splineFunction.at(next -> first).c - coeffs.c)/(3*h);
        m_splineFunction.insert(std::make_pair(x, coeffs));
        --i;
    }
}

Math::CSpline Math::NaturalCubicSplineInterpolator::getCoefficients() const
{
    return m_splineFunction;
}

std::pair<double, double> Math::NaturalCubicSplineInterpolator::interpolate(const std::map<double, double> &dataSet, double x)
{
    fitSpline(dataSet);

    auto it = m_splineFunction.begin();
    std::pair<double, double> interpolatedPoint;

    //Lower extrapolation
    if (x < it -> first)
    {
        std::cerr << "W: math::NaturalCubicSplineInterpolator::interpolate : query value " << x
                  << " outside data set. Lower extrapolation is being applied." << std::endl;

        const double deltax = x - it -> first;
        const CSCoeffs thisCoeff = it -> second;

        auto next = it;
        ++next;
        const CSCoeffs nextCoeff = next -> second;

        //Extrapolate using first cubic a, b, d coefficients
        //Preserve convexity by using second c coefficient from left endpoint
        const double y = thisCoeff.a + thisCoeff.b*deltax + nextCoeff.c*deltax*deltax + thisCoeff.d*deltax*deltax*deltax;
        interpolatedPoint = std::make_pair(x, y);
    }
    else
    {
        //Interpolation
        for (; it != m_splineFunction.end(); ++it)
        {
            if (it -> first > x)
            {
                auto previous = it;
                --previous;
                const double deltax = x - previous -> first;
                const CSCoeffs prevCoeff = previous -> second;

                const double y = prevCoeff.a + prevCoeff.b*deltax + prevCoeff.c*deltax*deltax + prevCoeff.d*deltax*deltax*deltax;
                interpolatedPoint = std::make_pair(x, y);
                break;
            }
        }

        //Upper extrapolation
        if (it == m_splineFunction.end())
        {
            std::cerr << "W: math::NaturalCubicSplineInterpolator::interpolate : query value " << x
                      << " outside data set. Upper extrapolation is being applied." << std::endl;

            --it;
            const double deltax = x - it -> first;
            const CSCoeffs thisCoeff = it -> second;

            auto previous = it;
            --previous;
            const CSCoeffs prevCoeff = previous -> second;
            const double h = it -> first - previous -> first;
            const double b = prevCoeff.b + 2*prevCoeff.c*h + 3*prevCoeff.d*h*h; //First derivative at right endpoint

            //Extrapolate using right-most cubic d, c coefficients
            //Continuity is guaranteed by appropriate choice of a
            //Differentiability is guaranteed by enforcing a condition on b
            const double y = thisCoeff.a + b*deltax + prevCoeff.c*deltax*deltax + prevCoeff.d*deltax*deltax*deltax;
            interpolatedPoint = std::make_pair(x, y);
        }
    }
    return interpolatedPoint;
}

std::map<double, double> Math::NaturalCubicSplineInterpolator::interpolatePoints(const std::map<double, double> &dataSet,
                                                             const std::set<double> &queryPoints)
{
    std::map<double, double> interpolatedPoints;
    for (const auto& it : queryPoints)
        interpolatedPoints.insert(interpolate(dataSet, it));

    return interpolatedPoints;
}

std::unique_ptr<Math::Interpolator> Math::NaturalCubicSplineInterpolator::clone() const
{
    return std::make_unique<Math::NaturalCubicSplineInterpolator>(*this);
}