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
        SeasonalDecompose(unsigned int period);
        virtual void decompose(const Common::TimeSeries &ts) = 0;

        virtual Common::TimeSeries getTrend() const = 0;
        virtual Common::TimeSeries getSeason() const = 0;
        virtual Common::TimeSeries getNoise() const = 0;

        unsigned int getPeriod() const;

        virtual std::unique_ptr<Common::SeasonalDecompose> clone() const = 0;

        virtual ~SeasonalDecompose() = default;

    protected:
        static const std::string trendNamePostfix, seasonNamePostfix, noiseNamePostfix;

    private:
        const unsigned int m_period;
    };

    class SeasonalDecomposeConvolution : public SeasonalDecompose
    {
    public:
        SeasonalDecomposeConvolution(unsigned int period);

        void decompose(const Common::TimeSeries &ts) final;

        Common::TimeSeries getTrend() const final;
        Common::TimeSeries getSeason() const final;
        Common::TimeSeries getNoise() const final;

        std::unique_ptr<Common::SeasonalDecompose> clone() const = 0;

    protected:
        void _extractTrend(const Common::TimeSeries &ts);
        void _extractSeason(const Common::TimeSeries &ts);

        virtual Common::TimeSeries _deTrend(const Common::TimeSeries &ts) = 0;
        virtual Common::TimeSeries _extractNoise(const Common::TimeSeries &ts) = 0;

    private:
        Common::TimeSeries m_trend, m_season, m_noise;
        //const unsigned int m_period;
    };

    class SeasonalDecomposeConvolutionAdditive : public SeasonalDecomposeConvolution
    {
    public:
        SeasonalDecomposeConvolutionAdditive(unsigned int period);

        std::unique_ptr<Common::SeasonalDecompose> clone() const final;

    protected:
        Common::TimeSeries _deTrend(const Common::TimeSeries &ts) final;
        Common::TimeSeries _extractNoise(const Common::TimeSeries &ts) final;
    };

    class SeasonalDecomposeConvolutionMultiplicative : public SeasonalDecomposeConvolution
    {
    public:
        SeasonalDecomposeConvolutionMultiplicative(unsigned int period);

        std::unique_ptr<Common::SeasonalDecompose> clone() const final;

    protected:
        Common::TimeSeries _deTrend(const Common::TimeSeries &ts) final;
        Common::TimeSeries _extractNoise(const Common::TimeSeries &ts) final;
    };


    class SeasonalDecomposeFactory
    {
    public:
        virtual std::unique_ptr<Common::SeasonalDecompose> create(unsigned int) const = 0;

        virtual ~SeasonalDecomposeFactory() = default;
    };

    class SeasonalDecomposeConvolutionAdditiveFactory : public SeasonalDecomposeFactory
    {
    public:
        std::unique_ptr<Common::SeasonalDecompose> create(unsigned int) const final;
    };

    class SeasonalDecomposeConvolutionMultiplicativeFactory : public SeasonalDecomposeFactory
    {
    public:
        std::unique_ptr<Common::SeasonalDecompose> create(unsigned int) const final;
    };
}


#endif //WILDCATSTKCORE_SEASONALDECOMPOSE_H
