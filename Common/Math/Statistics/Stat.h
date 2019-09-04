//
// Created by Alberto Campi on 14/04/2018.
//

#ifndef WILDCATSTKCORE_STAT_H
#define WILDCATSTKCORE_STAT_H

#include <vector>
#include <queue>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace Math
{
    class MultivariateStat
    {
    public:
        explicit MultivariateStat(unsigned int dimension);

        void add(const std::vector<double> &randomVector);
        std::vector<double> mean() const;
        std::vector<double> variance() const;
        std::vector<double> stdDev() const;
        std::vector<double> stdError() const;

        boost::numeric::ublas::matrix<double> covariance() const;
        boost::numeric::ublas::matrix<double> correlation() const;

    private:
        const unsigned int m_dim;
        unsigned long m_counter;

        std::vector<double> m_sumOfRVs;
        boost::numeric::ublas::matrix<double> m_sumOfCrossProdRVs;

    };


    class UnivariateStat
    {
    public:
        UnivariateStat();

        void add(double x);
        double mean() const;
        double variance() const;
        double stdDev() const;
        double stdError() const;

    private:
        unsigned long m_counter;

        double m_sumOfRV;
        double m_sumOfSquaredRV;

    };

    class MovingAverage
    {
    public:
        MovingAverage(unsigned int window);

        virtual void add(double x);
        virtual double get() const;

    private:
        const unsigned int m_size;
        double m_accum;
        std::queue<double> m_buffer;

    protected:
        unsigned int _getWindowSize() const;
        double _getAccumulator() const;
    };

    class CenteredMovingAverage : public MovingAverage
    {
    public:
        CenteredMovingAverage(unsigned int window);

        void add(double x) override;
        double get() const override;

    private:
        double m_leftAccum;
        int m_leftAccumCtr;
    };


    void checkDivisionByZero(double denominator);
}
#endif //WILDCATSTKCORE_STAT_H
