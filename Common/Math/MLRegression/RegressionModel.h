//
// Created by Alberto Campi on 2019-08-03.
//

#ifndef WILDCATSTKCORE_REGRESSIONMODEL_H
#define WILDCATSTKCORE_REGRESSIONMODEL_H

#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>


namespace Math
{
    class RegressionModel
    {
    public:
        virtual void calibrate(std::vector<double> &coefficients,
                               const std::vector<double> &dependentVariableValues,
                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const = 0;

        virtual std::unique_ptr<Math::RegressionModel> clone() const = 0;

        virtual ~RegressionModel() = default;
    };

    class RegressionModelOLS : public RegressionModel
    {
    public:
        //RegressionModelOLS(algoritmObject);
        void calibrate(std::vector<double> &coefficients,
                       const std::vector<double> &dependentVariableValues,
                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;

        std::unique_ptr<Math::RegressionModel> clone() const final;

    private:
        //pointer to algorithm implementation
    };

class RegressionModelAlgorithm
{
public:
    virtual void calibrate(std::vector<double>& coefficients,
                           const std::vector<double>& dependentVariableValues,
                           const boost::numeric::ublas::matrix<double>& independentVariableValues) const = 0;

    virtual ~RegressionModelAlgorithm() = default;

    virtual std::unique_ptr<Math::RegressionModelAlgorithm> clone() const = 0;
};


class RegressionModelAlgorithmMoorePenrose : public RegressionModelAlgorithm
{
public:
    void calibrate(std::vector<double>& coefficients,
                   const std::vector<double>& dependentVariableValues,
                   const boost::numeric::ublas::matrix<double>& independentVariableValues) const final;

    std::unique_ptr<Math::RegressionModelAlgorithm> clone() const final;
};

}
#endif //WILDCATSTKCORE_REGRESSIONMODEL_H
