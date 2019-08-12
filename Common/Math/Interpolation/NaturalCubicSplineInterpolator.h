//
// Created by Alberto Campi on 10/12/2018.
//

#ifndef QUANTLIB_NATURALCUBICSPLINEINTERPOLATOR_H
#define QUANTLIB_NATURALCUBICSPLINEINTERPOLATOR_H

#include "Interpolator.h"

namespace Math
{
    class NaturalCubicSplineInterpolator : public Interpolator
    {
    public:
        NaturalCubicSplineInterpolator() = default;

        std::pair<double, double> interpolate(const std::map<double, double>& dataSet, double x) override;
        std::map <double, double> interpolatePoints(const std::map<double, double>& dataSet,
                                                    const std::set<double>& queryPoints) override;

        std::unique_ptr<Math::Interpolator> clone() const final;

        void fitSpline(const std::map<double, double>& dataSet);
        CSpline getCoefficients() const;


    private:
        CSpline m_splineFunction;
        std::map<double, double> m_dataSet;

    };
}


#endif //QUANTLIB_NATURALCUBICSPLINEINTERPOLATOR_H
