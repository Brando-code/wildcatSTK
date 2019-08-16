//
// Created by Alberto Campi on 11/11/2018.
//

#ifndef QUANTLIB_INTERPOLATOR_H
#define QUANTLIB_INTERPOLATOR_H

#include <map>
#include <set>

namespace Math
{
    class Interpolator
    {
    public:
        virtual std::pair<double, double> interpolate(const std::map<double, double>& dataSet, double x) = 0;
        virtual std::map<double, double> interpolatePoints(const std::map<double, double>& dataSet,
                                                           const std::set<double>& queryPoints) = 0;

        virtual std::unique_ptr<Math::Interpolator> clone() const = 0;

        virtual ~Interpolator() = default;

    };


    //Data structure encapsulating cubic spline parameters
    struct CSCoeffs
    {
        CSCoeffs() = default;
        CSCoeffs(double aConst, double bLin, double cQuad, double dCub);
        double a;   //constant coefficient -> y-value at node
        double b;   //linear coefficient -> related to first derivative
        double c;   //quadratic coefficient -> related to second derivative
        double d;   //cubic coefficient

        CSCoeffs& operator=(const CSCoeffs& rhsCSCoeffs) = default;
        bool operator==(const CSCoeffs& rhsCSCoeffs);
        bool operator!=(const CSCoeffs& rhsCSCoeffs);
    };


    using CSpline = std::map<double, Math::CSCoeffs>;


    //Abstract factory classes
    class InterpolatorFactory
    {
    public:
        virtual std::unique_ptr<Math::Interpolator> create() const = 0;

        virtual ~InterpolatorFactory() = default;
    };

    class LinearInterpolatorFactory : public InterpolatorFactory
    {
    public:
        std::unique_ptr<Math::Interpolator> create() const final;
    };

    class NaturalCubicSplineInterpolatorFactory : public InterpolatorFactory
    {
    public:
        std::unique_ptr<Math::Interpolator> create() const final;
    };
}


#endif //QUANTLIB_INTERPOLATOR_H
