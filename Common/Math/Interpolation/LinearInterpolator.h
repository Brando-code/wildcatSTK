//
// Created by Alberto Campi on 11/11/2018.
//

#ifndef QUANTLIB_LINEARINTERPOLATOR_H
#define QUANTLIB_LINEARINTERPOLATOR_H

#include "Interpolator.h"

namespace Math
{
    class LinearInterpolator : public Interpolator
    {
    public:
        std::pair<double, double> interpolate(const std::map<double, double>& dataSet, double x) override;
        std::map<double, double> interpolatePoints(const std::map<double, double>& dataSet,
                                                   const std::set<double>& queryPoints) override;
        std::unique_ptr<Math::Interpolator> clone() const final;
    };

}


#endif //QUANTLIB_LINEARINTERPOLATOR_H
