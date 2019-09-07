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
    boost::numeric::ublas::matrix<double> computeInverseMatrix (boost::numeric::ublas::matrix<double> inputMatrix);
    int computeDeterminantSign (const boost::numeric::ublas::permutation_matrix<std::size_t> &permutationMatrix);
    double computeMatrixDeterminant (boost::numeric::ublas::matrix<double> inputMatrix);
    boost::numeric::ublas::matrix<double> choleskyDecompose(const boost::numeric::ublas::matrix<double> &inputMatrix);


    struct SummaryStatistic
    {
        double stdErr;
        double tRatio;
        double pValue;
    };

    struct ANOVA
    {
        std::vector<Math::SummaryStatistic> coefficientSummaryStat;

        boost::numeric::ublas::vector<double> fittedValues;
        boost::numeric::ublas::vector<double> residuals;

        double sampleSize;
        double residualMSEVariance;
        double residualDoF;
        double modelMSEVariance;
        double modelDoF;
        double totalMSEVariance;
        double totalMean;
        double totalDoF;
        double RSquared;
        double adjRSquared;
    };

    class RegressionModelAlgorithm;

    class RegressionModel
    {
    public:
        virtual void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                               const boost::numeric::ublas::vector<double> &dependentVariableValues,
                               const boost::numeric::ublas::matrix<double> &independentVariableValues) const = 0;
        virtual void computeANOVA() = 0;
        Math::ANOVA getANOVA() const;

        virtual std::unique_ptr<Math::RegressionModel> clone() const = 0;

        virtual ~RegressionModel() = default;

    protected:
        mutable boost::numeric::ublas::vector<double> m_coefficients, m_depVariableVals;
        mutable boost::numeric::ublas::matrix<double> m_indepVariableVals;
        Math::ANOVA m_anova;

        void _ANOVATotal();
        void _ANOVAResiduals();
        void _ANOVAModel();
        void _ANOVARSquared();
    };

    class RegressionModelOLS : public RegressionModel
    {
    public:
        RegressionModelOLS();
        RegressionModelOLS(const RegressionModelOLS& other);

        void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                       const boost::numeric::ublas::vector<double> &dependentVariableValues,
                       const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;
        void computeANOVA() final;

        std::unique_ptr<Math::RegressionModel> clone() const final;

    private:
        std::unique_ptr<Math::RegressionModelAlgorithm> m_algorithmPtr;
    };


class RegressionModelAlgorithm
{
public:
    virtual void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                           const boost::numeric::ublas::vector<double> &dependentVariableValues,
                           const boost::numeric::ublas::matrix<double> &independentVariableValues) const = 0;

    virtual ~RegressionModelAlgorithm() = default;

    virtual std::unique_ptr<Math::RegressionModelAlgorithm> clone() const = 0;
};

class RegressionModelAlgorithmMoorePenrose : public RegressionModelAlgorithm
{
public:
    void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                   const boost::numeric::ublas::vector<double> &dependentVariableValues,
                   const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;

    std::unique_ptr<Math::RegressionModelAlgorithm> clone() const final;
};

class RegressionModelAlgorithmCholesky : public RegressionModelAlgorithm
{
public:
    void calibrate(boost::numeric::ublas::vector<double> &coefficients,
                   const boost::numeric::ublas::vector<double> &dependentVariableValues,
                   const boost::numeric::ublas::matrix<double> &independentVariableValues) const final;

    std::unique_ptr<Math::RegressionModelAlgorithm> clone() const final;
};


}
#endif //WILDCATSTKCORE_REGRESSIONMODEL_H
