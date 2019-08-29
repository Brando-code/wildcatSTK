//
// Created by Alberto Campi on 2019-08-27.
//

#ifndef WILDCATSTKCORE_SEASONALDECOMPOSE_H
#define WILDCATSTKCORE_SEASONALDECOMPOSE_H

#include <vector>
#include "../Types/TimeSeries.h"


namespace Common
{
    class SeasonalDecompose
    {
    public:
        virtual void decompose(const Common::TimeSeries& ts) = 0;

        virtual Common::TimeSeries getTrend() const = 0;
        virtual Common::TimeSeries getSeason() const = 0;
        virtual Common::TimeSeries getNoise() const = 0;

        virtual ~SeasonalDecompose() = default;

    protected:
        static const std::string trendNamePostfix, seasonNamePostfix, noiseNamePostfix;
    };

    class SeasonalDecomposeConvolution : public SeasonalDecompose
    {
    public:
        SeasonalDecomposeConvolution(unsigned int period);

        void decompose(const Common::TimeSeries& ts) final;
        Common::TimeSeries getTrend() const final;
        Common::TimeSeries getSeason() const final;
        Common::TimeSeries getNoise() const final;

    protected:
        void _extractTrend(const Common::TimeSeries& ts);
        void _extractSeason(const Common::TimeSeries& ts);
        //unsigned int _getPeriod() const;

        virtual Common::TimeSeries _deTrend(const Common::TimeSeries& ts) = 0;
        virtual Common::TimeSeries _extractNoise(const Common::TimeSeries& ts) = 0;

    private:
        Common::TimeSeries m_trend, m_season, m_noise;
        const unsigned int m_period;
    };

    class SeasonalDecomposeConvolutionAdditive : public SeasonalDecomposeConvolution
    {
    public:
        SeasonalDecomposeConvolutionAdditive(unsigned int period);

    protected:
        Common::TimeSeries _deTrend(const Common::TimeSeries& ts) final;
        Common::TimeSeries _extractNoise(const Common::TimeSeries& ts) final;
    };
}


#endif //WILDCATSTKCORE_SEASONALDECOMPOSE_H
