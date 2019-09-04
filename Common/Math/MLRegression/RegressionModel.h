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
    boost::numeric::ublas::matrix<double> inverseBST (const boost::numeric::ublas::matrix<double> &inputMatrix);
    int determinantSign (const boost::numeric::ublas::permutation_matrix<std::size_t> &permutationMatrix);
    double matrixDeterminant (boost::numeric::ublas::matrix<double> inputMatrix);
    boost::numeric::ublas::matrix<double> choleskyDecomp(const boost::numeric::ublas::matrix<double> &inputMatrix);
    boost::numeric::ublas::vector<double> stdVec2bstVec (const std::vector<double> &inputVector);
    std::vector<double> bstVec2stdVec (const boost::numeric::ublas::vector<double> &inputVector);

    class RegressionModelAlgorithm;

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
        RegressionModelOLS();
        RegressionModelOLS(const RegressionModelOLS& other);
        //RegressionModelOLS(const RegressionModelAlgorithm& regressionAlgorithm);
        void calibrate(std::vector<double> &coefficients,
                       const std::vector<double> &dependentVariableValues,
                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;

        std::unique_ptr<Math::RegressionModel> clone() const final;

    private:
        std::unique_ptr<Math::RegressionModelAlgorithm> m_algorithmPtr;
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

    class RegressionModelAlgorithmCholesky : public RegressionModelAlgorithm
    {
    public:
        void calibrate(std::vector<double>& coefficients,
                       const std::vector<double>& dependentVariableValues,
                       const boost::numeric::ublas::matrix<double>& independentVariableValues) const final;

        std::unique_ptr<Math::RegressionModelAlgorithm> clone() const final;
    };


}
#endif //WILDCATSTKCORE_REGRESSIONMODEL_H
