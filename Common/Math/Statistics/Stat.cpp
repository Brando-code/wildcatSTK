//
// Created by Alberto Campi on 14/04/2018.
//

#include "Stat.h"
#include <cmath>

void Math::checkDivisionByZero(double denominator)
{
    if (denominator == 0)
        throw std::overflow_error("Math::checkDivisionByZero : Divide by zero exception");
}

Math::MultivariateStat::MultivariateStat(unsigned int dimension) : m_dim(dimension), m_counter(0), m_sumOfRVs(dimension),
                                     m_sumOfCrossProdRVs(dimension, dimension, 0)
{

}

void Math::MultivariateStat::add(const std::vector<double>& randomVariables)
{
    if (randomVariables.size() != m_dim)
        throw std::out_of_range("Stat::add : argument size does not match object dimension.");

    for (unsigned int i = 0; i < m_dim; ++i)
    {
        m_sumOfRVs[i] += randomVariables[i];
        for (unsigned int j = i; j < m_dim; ++j)
            m_sumOfCrossProdRVs(i, j) = m_sumOfCrossProdRVs(j, i)  += randomVariables[i]*randomVariables[j];
    }
    ++m_counter;
}

std::vector<double> Math::MultivariateStat::mean() const
{
    Math::checkDivisionByZero(m_counter);
    std::vector<double> meanValues;
    for (const auto &values : m_sumOfRVs)
        meanValues.push_back(values / static_cast<double>(m_counter));

    return meanValues;
}

std::vector<double> Math::MultivariateStat::variance() const
{
    Math::checkDivisionByZero(m_counter);
    std::vector<double> varianceValues;
    for (unsigned int i = 0; i < m_dim; ++i)
    {
        const double var = (m_sumOfCrossProdRVs(i, i) - m_sumOfRVs[i] * m_sumOfRVs[i] / static_cast<double>(m_counter))
                / static_cast<double>(m_counter);
        varianceValues.push_back(var);
    }

    return varianceValues;
}

std::vector<double> Math::MultivariateStat::stdDev() const
{
    Math::checkDivisionByZero(m_counter);
    std::vector<double> stdDevValues;
    for (unsigned int i = 0; i < m_dim; ++i)
    {
        const double stdDev = sqrt(
                (m_sumOfCrossProdRVs(i, i) - m_sumOfRVs[i] * m_sumOfRVs[i] / static_cast<double>(m_counter))
                / static_cast<double>(m_counter));
        stdDevValues.push_back(stdDev);
    }
    return stdDevValues;
}

std::vector<double> Math::MultivariateStat::stdError() const
{
    Math::checkDivisionByZero(m_counter);
    std::vector<double> stdError;
    for (unsigned int i = 0; i < m_dim; ++i)
    {
        const double stdErr =
                sqrt((m_sumOfCrossProdRVs(i, i) - m_sumOfRVs[i] * m_sumOfRVs[i] / static_cast<double>(m_counter))
                / static_cast<double>(m_counter)) /
                sqrt(static_cast<double>(m_counter));
        stdError.push_back(stdErr);
    }
    return stdError;
}

boost::numeric::ublas::matrix<double> Math::MultivariateStat::covariance() const
{
    Math::checkDivisionByZero(m_counter);
    boost::numeric::ublas::matrix<double> covarianceValues(m_dim, m_dim);
    for (unsigned int row = 0; row < m_dim; ++row)
        for (unsigned int column = 0; column <= row; ++column) {
            const double covar =
                    (m_sumOfCrossProdRVs(row, column) - m_sumOfRVs[row] * m_sumOfRVs[column] / static_cast<double>(m_counter))
                    / static_cast<double>(m_counter);
            covarianceValues(row, column) = covarianceValues(column, row) = covar;

    }
    return covarianceValues;
}

boost::numeric::ublas::matrix<double> Math::MultivariateStat::correlation() const
{
    Math::checkDivisionByZero(m_counter);
    boost::numeric::ublas::matrix<double> correlationValues(m_dim, m_dim);
    for (unsigned int row = 0; row < m_dim; ++row)
        for (unsigned int column = 0; column <= row; ++column)
        {
            const double covar =
                    (m_sumOfCrossProdRVs(row, column) - m_sumOfRVs[row]*m_sumOfRVs[column]/static_cast<double>(m_counter))
                    /static_cast<double>(m_counter);
            const double rowStdDev =
                    sqrt((m_sumOfCrossProdRVs(row, row)- m_sumOfRVs[row]*m_sumOfRVs[row]/static_cast<double>(m_counter))
                    /static_cast<double>(m_counter));
            const double columnStdDev =
                    sqrt((m_sumOfCrossProdRVs(column, column)- m_sumOfRVs[column]*m_sumOfRVs[column]/static_cast<double>(m_counter))
                    /static_cast<double>(m_counter));
            correlationValues(row, column) = correlationValues(column, row)  = covar/(rowStdDev*columnStdDev);
        }

    return correlationValues;
}


Math::UnivariateStat::UnivariateStat() : m_counter(0), m_sumOfRV(0), m_sumOfSquaredRV(0)
{

}

void Math::UnivariateStat::add(double x)
{
    m_sumOfRV += x;
    m_sumOfSquaredRV += x*x;
    ++m_counter;
}

double Math::UnivariateStat::mean() const
{
    Math::checkDivisionByZero(m_counter);
    return m_sumOfRV/static_cast<double>(m_counter);
}

double Math::UnivariateStat::variance() const
{
    Math::checkDivisionByZero(m_counter);
    return (m_sumOfSquaredRV - m_sumOfRV*m_sumOfRV/ static_cast<double>(m_counter))/ static_cast<double>(m_counter);
}

double Math::UnivariateStat::stdDev() const
{
    return sqrt(variance());
}

double Math::UnivariateStat::stdError() const
{
    return stdDev()/sqrt(static_cast<double>(m_counter));
}

Math::MovingAverage::MovingAverage(unsigned int window) : m_size(window), m_buffer(), m_accum(0) {}

void Math::MovingAverage::add(double x)
{
    if (m_buffer.size() >= m_size)
    {
        m_accum -= m_buffer.front();
        m_buffer.pop();
    }

    m_buffer.push(x);
    m_accum += x;
}

double Math::MovingAverage::get() const
{
    if (m_buffer.size() < m_size)
        return std::nan("");
    else
        return m_accum/m_buffer.size();
}

double Math::MovingAverage::_getAccumulator() const
{
    return m_accum;
}

unsigned int Math::MovingAverage::_getWindowSize() const
{
    return m_size;
}

Math::CenteredMovingAverage::CenteredMovingAverage(unsigned int window) : MovingAverage(window), m_leftAccumCtr(-1)
{}

void Math::CenteredMovingAverage::add(double x)
{
    m_leftAccum = _getAccumulator();
    ++m_leftAccumCtr;
    Math::MovingAverage::add(x);
}

double Math::CenteredMovingAverage::get() const
{
    if (m_leftAccumCtr < _getWindowSize())
        return std::nan("");
    else
        return 1./(_getWindowSize() * 2) * (_getAccumulator() + m_leftAccum);
}