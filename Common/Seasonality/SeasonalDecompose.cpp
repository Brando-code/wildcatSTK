//
// Created by Alberto Campi on 2019-08-27.
//

#include "SeasonalDecompose.h"
#include "../Math/Statistics/Stat.h"

const std::string Common::SeasonalDecompose::trendNamePostfix = "_TD";
const std::string Common::SeasonalDecompose::seasonNamePostfix = "_SEAS";
const std::string Common::SeasonalDecompose::noiseNamePostfix = "_NO";

Common::SeasonalDecomposeConvolution::SeasonalDecomposeConvolution(unsigned int period) : m_period(period)
{}

Common::TimeSeries Common::SeasonalDecomposeConvolution::getTrend() const
{
    return m_trend;
}

Common::TimeSeries Common::SeasonalDecomposeConvolution::getSeason() const
{
    return m_season;
}

Common::TimeSeries Common::SeasonalDecomposeConvolution::getNoise() const
{
    return m_noise;
}

void Common::SeasonalDecomposeConvolution::decompose(const Common::TimeSeries &ts)
{
    if (ts.getValues().size() < 2 * m_period)
        throw std::runtime_error("Common::SeasonalDecomposeConvolution::decompose : data should cover at least two seasonal cycles");

    _extractTrend(ts);
    const Common::TimeSeries deTrended = _deTrend(ts);

    _extractSeason(deTrended);
    const Common::TimeSeries noise = _extractNoise(ts);
    m_noise.set(ts.getName() + Common::SeasonalDecompose::noiseNamePostfix, noise.getValues(), noise.getDates());
}

void Common::SeasonalDecomposeConvolution::_extractTrend(const Common::TimeSeries &ts)
{
    const unsigned int dim = 2;
    Math::CenteredMovingAverage cma(m_period);

    std::vector<double> trend;
    for (unsigned int i = 0; i < ts.getValues().size(); ++i)
    {
        cma.add(ts.getValue(i));
        if (!std::isnan(cma.get()))
            trend.push_back(cma.get());
    }

    Math::MultivariateStat head(dim), tail(dim);
    for (unsigned int i = 0; i < m_period; ++i)
    {
        head.add({static_cast<double>(i), trend.at(i)});
        tail.add({static_cast<double>(trend.size() - i - 1), trend.at(trend.size() - i - 1)});
    }

    const double betaHatHead = head.covariance()(0, 1) / head.variance().at(0);
    const double betaHatTail = tail.covariance()(0, 1) / tail.variance().at(0);
    const double alphaHatHead = head.mean().at(1) - betaHatHead * head.mean().at(0);
    const double alphaHatTail = tail.mean().at(1) - betaHatTail * tail.mean().at(0);

    const int extrapWindowSize = m_period / 2;
    std::vector<double> headExtrapolation;
    for (int i = 0; i < extrapWindowSize; ++i)
    {
        const double headValue = (i - extrapWindowSize) * betaHatHead + alphaHatHead;
        headExtrapolation.push_back(headValue);

        const double tailValue = (trend.size() + i) * betaHatTail + alphaHatTail;
        trend.push_back(tailValue);
    }

    headExtrapolation.insert(headExtrapolation.end(), trend.begin(), trend.end());
    m_trend.set(ts.getName() + SeasonalDecompose::trendNamePostfix, headExtrapolation, ts.getDates());
}

void Common::SeasonalDecomposeConvolution::_extractSeason(const Common::TimeSeries &ts)
{
    //implement by using mod arithmetic
    std::vector<double> accumulators(m_period, 0);
    std::vector<unsigned long> counters(m_period, 0);

    for (unsigned long i = 0; i < ts.getValues().size(); ++i)
    {
        unsigned long index;
        if (i < m_period)
            index = i;
        else
            index = i % m_period;

        accumulators.at(index) += ts.getValue(i);
        counters.at(index) += 1;
    }

    std::vector<double> seasonality(ts.getValues().size());
    for (unsigned long i = 0; i < ts.getValues().size(); ++i)
    {
        unsigned long index;
        if (i < m_period)
            index = i;
        else
            index = i % m_period;

        seasonality.at(i) = accumulators.at(index) / counters.at(index);
    }

    m_season.set(ts.getName() + SeasonalDecompose::seasonNamePostfix, seasonality, ts.getDates());
}
/*
unsigned int Common::SeasonalDecomposeConvolution::_getPeriod() const
{
    return m_period;
}
*/
Common::SeasonalDecomposeConvolutionAdditive::SeasonalDecomposeConvolutionAdditive(unsigned int period) :
    SeasonalDecomposeConvolution(period)
{}

Common::TimeSeries Common::SeasonalDecomposeConvolutionAdditive::_deTrend(const Common::TimeSeries &ts)
{
    return ts - getTrend();
}

Common::TimeSeries Common::SeasonalDecomposeConvolutionAdditive::_extractNoise(const Common::TimeSeries &ts)
{
    return ts - getTrend() - getSeason();
}