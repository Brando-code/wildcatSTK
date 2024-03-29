//
// Created by Alberto Campi on 2019-08-03.
//

#ifndef WILDCATSTKCORE_REGRESSIONMODEL_H
#define WILDCATSTKCORE_REGRESSIONMODEL_H

#include <boost/numeric/ublas/matrix.hpp>
#include "../LinearAlgebra/MatrixDecompose.h"


namespace Math
{
    class RegressionModelAlgorithm;

    //
    // Regression ANOVA summary stat data structs (rvs to client code) and computation
    //
    struct SummaryStatistic
    {
        double stdErr;
        double tRatio;
        double pValue;
    };

    struct ANOVASummary
    {
        std::vector<Math::SummaryStatistic> coefficientSummaryStat;

        boost::numeric::ublas::vector<double> fittedValues;
        boost::numeric::ublas::vector<double> residuals;

        double sampleSize;
        double residualMSEVariance; // Reduced chi-squared
        double regressionStdErr;
        double residualDoF;
        double modelMSEVariance;
        double modelDoF;
        double totalMSEVariance;
        double totalMean;
        double totalDoF;
        double RSquared;
        double adjRSquared;
    };

    class ANOVA
    {
    public:

        Math::ANOVASummary computeANOVA(const boost::numeric::ublas::vector<double> &coefficients,
                                        const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                        const boost::numeric::ublas::matrix<double> &independentVariableValues,
                                        const Math::RegressionModelAlgorithm &reg) const;
    };


    //
    // Regression model class hierarchy including OLS
    //
    class RegressionModel
    {
    public:
        virtual void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                               const boost::numeric::ublas::vector<double> &dependentVariableValues,
                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const = 0;
        virtual Math::ANOVASummary getANOVA() const = 0;

        virtual std::unique_ptr<Math::RegressionModel> clone() const = 0;

        virtual ~RegressionModel() = default;
    };

    class RegressionModelOLS : public RegressionModel
    {
    public:
        RegressionModelOLS();
        RegressionModelOLS(const RegressionModelOLS& other);
        RegressionModelOLS& operator=(const RegressionModelOLS& other);

        void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                       const boost::numeric::ublas::vector<double> &dependentVariableValues,
                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;
        Math::ANOVASummary getANOVA() const final;

        std::unique_ptr<Math::RegressionModel> clone() const final;

    private:
        mutable std::unique_ptr<Math::RegressionModelAlgorithm> m_algorithmPtr;
    };


    //
    // Recursive descent chain of responsibility classes for OLS linear system solution
    //
    class RegressionModelAlgorithmOLSChain
    {
    public:
        RegressionModelAlgorithmOLSChain();
        virtual std::unique_ptr<Math::RegressionModelAlgorithm> handle(boost::numeric::ublas::vector<double> &coefficients,
                                                                       const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const;

        void addLink(const std::shared_ptr<Math::RegressionModelAlgorithmOLSChain> &link);

        virtual ~RegressionModelAlgorithmOLSChain() = default;

    private:
        std::shared_ptr<Math::RegressionModelAlgorithmOLSChain> m_nextLink;
    };

    class RegressionModelOLSLinkCholesky : public RegressionModelAlgorithmOLSChain
    {
        std::unique_ptr<Math::RegressionModelAlgorithm> handle(boost::numeric::ublas::vector<double> &coefficients,
                                                               const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;
    };

    class RegressionModelOLSLinkMoorePenrose : public RegressionModelAlgorithmOLSChain
    {
        std::unique_ptr<Math::RegressionModelAlgorithm> handle(boost::numeric::ublas::vector<double> &coefficients,
                                                               const boost::numeric::ublas::vector<double> &dependentVariableValues,
                                                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;
    };


    //
    // Regression model algorithm class hierarchy
    //
    class RegressionModelAlgorithm
    {
    public:
        virtual void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                               const boost::numeric::ublas::vector<double> &dependentVariableValues,
                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const = 0;
        virtual bool hasFailed() const = 0;
        virtual boost::numeric::ublas::matrix<double> computeCoefficientCovarianceMatrix(double residualVariance) const = 0;
        Math::ANOVASummary getANOVA() const;

        virtual ~RegressionModelAlgorithm() = default;

        virtual std::unique_ptr<Math::RegressionModelAlgorithm> clone() const = 0;

    protected:
        mutable boost::numeric::ublas::vector<double> m_coefficients, m_depVariableVals;
        mutable boost::numeric::ublas::matrix<double> m_indepVariableVals;
    };

    class RegressionModelAlgorithmMoorePenrose : public RegressionModelAlgorithm
    {
    public:
        RegressionModelAlgorithmMoorePenrose();
        void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                       const boost::numeric::ublas::vector<double> &dependentVariableValues,
                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;
        bool hasFailed() const final;
        boost::numeric::ublas::matrix<double> computeCoefficientCovarianceMatrix(double residualVariance) const final;

        std::unique_ptr<Math::RegressionModelAlgorithm> clone() const final;

    private:
        mutable boost::numeric::ublas::matrix<double> m_invXtX;
        mutable bool m_isInvertible;

        void _computeInverseByLUFactorization(boost::numeric::ublas::matrix<double> M,
                                              boost::numeric::ublas::matrix<double>& M_inv) const;
    };

    class RegressionModelAlgorithmCholesky : public RegressionModelAlgorithm
    {
    public:
        void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                       const boost::numeric::ublas::vector<double> &dependentVariableValues,
                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;
        bool hasFailed() const final;
        boost::numeric::ublas::matrix<double> computeCoefficientCovarianceMatrix(double residualVariance) const final;

        std::unique_ptr<Math::RegressionModelAlgorithm> clone() const final;

    private:
        mutable Math::CholeskyDecompose m_ch;

        boost::numeric::ublas::vector<double> _choleskySolve(const boost::numeric::ublas::matrix<double> &choleskyFactor,
                                                             const boost::numeric::ublas::vector<double> &rhs) const;
        boost::numeric::ublas::matrix<double> _choleskySolve(const boost::numeric::ublas::matrix<double> &choleskyFactor,
                                                             const boost::numeric::ublas::matrix<double> &rhs) const;
    };


}
#endif //WILDCATSTKCORE_REGRESSIONMODEL_H
