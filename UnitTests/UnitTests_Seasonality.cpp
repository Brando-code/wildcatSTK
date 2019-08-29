//
// Created by Alberto Campi on 2019-08-27.
//
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
//

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "Utils.h"
#include <cmath>
#include "../Common/Seasonality/SeasonalDecompose.h"
#include "../Common/Types/DataSet.h"
#include "../Common/Utils/IO/JSONParser.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

const std::string expectedOutputRelativePath = "../UnitTests/Outputs/Expected/";
const std::string actualOutputRelativePath = "../UnitTests/Outputs/Actual/";
const std::string inputRelativePath = "../UnitTests/Inputs/";


BOOST_AUTO_TEST_SUITE(ConvolutionFilters)
    struct FixtureAdditive : public Common::SeasonalDecomposeConvolutionAdditive
    {
        FixtureAdditive(unsigned int window) : SeasonalDecomposeConvolutionAdditive(window) {};

        void extractTrend(const Common::TimeSeries& ts) {_extractTrend(ts);};
        void extractSeason(const Common::TimeSeries& ts) {_extractSeason(ts);};
    };

    void loadDataSet(const std::string& JSONfilePath, Common::DataSet& ds)
    {
        ds.clearAllData();
        Common::JSONParserDecoratorDataSet js;
        js.readJSON(JSONfilePath);
        ds = js.getDataSet();
    }

    const std::string fileName = "sample_dataSet_clean.json";

    BOOST_AUTO_TEST_CASE(Trend_happyPath, *utf::tolerance(1e-6))
    {
        const unsigned int window = 4;
        FixtureAdditive dec(window);

        Common::DataSet data;
        loadDataSet(inputRelativePath + fileName, data);

        const std::string name = "RPI_FOOD_DRINK_TOBACCO_NSA";
        const Common::TimeSeries ts = data.getTimeSeries(name);

        dec.extractTrend(ts);
        const Common::TimeSeries td = dec.getTrend();

        const std::string expectedFileName = "Conv_filter_td_happyPath_expected.txt";
        std::vector<double> expected;

        std::ifstream in;
        in.open(expectedOutputRelativePath + expectedFileName);
        readArray(expected, in, true);
        in.close();

        BOOST_TEST(td.getValues() == expected, tt::per_element());
    }

    BOOST_AUTO_TEST_CASE(Season_happyPath, *utf::tolerance(1e-6))
    {
        const unsigned int window = 4;
        FixtureAdditive dec(window);

        Common::DataSet data;
        loadDataSet(inputRelativePath + fileName, data);

        const std::string name = "RPI_FOOD_DRINK_TOBACCO_NSA";
        const Common::TimeSeries ts = data.getTimeSeries(name);

        dec.extractTrend(ts);
        const Common::TimeSeries td = dec.getTrend();
        const Common::TimeSeries detd = ts - td;

        dec.extractSeason(detd);
        const Common::TimeSeries season = dec.getSeason();

        const std::string expectedFileName = "Conv_filter_seas_happyPath_expected.txt";
        std::ifstream in;
        in.open(expectedOutputRelativePath + expectedFileName);
        std::vector<double> expected;
        readArray(expected, in, true);

        BOOST_TEST(season.getValues() == expected, tt::per_element());
    }

    BOOST_AUTO_TEST_CASE(Decompose_happyPath, *utf::tolerance(1e-6))
    {
        const unsigned int window = 4;
        Common::SeasonalDecomposeConvolutionAdditive sdca(window);

        Common::DataSet data;
        loadDataSet(inputRelativePath + fileName, data);

        const std::string name = "RPI_FOOD_DRINK_TOBACCO_NSA";
        const Common::TimeSeries ts = data.getTimeSeries(name);

        sdca.decompose(ts);
        const std::string expectedTrendFileName = "Conv_filter_td_happyPath_expected.txt";
        const std::string expectedSeasonFileName = "Conv_filter_seas_happyPath_expected.txt";
        std::vector<double> expectedTrend, expectedSeason;

        std::ifstream in; in.open(expectedOutputRelativePath + expectedTrendFileName);
        readArray(expectedTrend, in, true);
        in.close();
        in.open(expectedOutputRelativePath + expectedSeasonFileName);
        readArray(expectedSeason, in, true);

        BOOST_TEST(sdca.getTrend().getValues() == expectedTrend, tt::per_element());
        BOOST_TEST(sdca.getSeason().getValues() == expectedSeason, tt::per_element());
        BOOST_TEST((sdca.getTrend() + sdca.getSeason() +
                    sdca.getNoise()).getValues() == ts.getValues(), tt::per_element());

        BOOST_TEST(sdca.getTrend().getDates() == sdca.getSeason().getDates(), tt::per_element());
        BOOST_TEST(sdca.getSeason().getDates() == sdca.getNoise().getDates(), tt::per_element());
        BOOST_TEST(sdca.getNoise().getDates() == ts.getDates(), tt::per_element());
    }

BOOST_AUTO_TEST_SUITE_END()



