//
// Created by Alberto Campi on 22/07/2019.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
//
#define BOOST_TEST_MODULE UnitTestsTypes
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "../wildcatSTKCoreIncludes.h"

#include <iostream>
#include <cmath>

namespace utf = boost::unit_test;


BOOST_AUTO_TEST_SUITE(ConfigVariable)
    const double tol = 1e-10;
    struct Fixture
    {
        Fixture() : f_variableName("US_GDP"), f_values({234.0, 346.0, 447.0, 531.0}),f_ts(f_variableName, f_values) {}
        ~Fixture() = default;

        Common::TimeSeries getTimeSeries() const { return f_ts;};

        const std::string f_variableName;
        const std::vector<double> f_values;
        const Common::TimeSeries f_ts;
    };


    BOOST_AUTO_TEST_CASE(ConfigVariable_firstDiff_happyPath)
    {
        const Fixture fx = Fixture();
        const std::string rawConfigVariable = "US_GDP|D|1";

        const Common::ConfigVariable cv = Common::ConfigVariable(rawConfigVariable);

        BOOST_CHECK_EQUAL(cv.getBasename(), fx.getTimeSeries().getName());
        BOOST_CHECK_EQUAL(cv.getTransformationTypeCode(), "D");
        BOOST_CHECK_EQUAL(cv.getLagDependency(), 1);

        const unsigned int index = 2;
        const double expectedTransformedValue = fx.getTimeSeries().getValues().at(index-1) - fx.getTimeSeries().getValues().at(index-2);
        BOOST_CHECK_EQUAL(cv.getTransformedVariableValue(fx.getTimeSeries(), index), expectedTransformedValue);
        BOOST_CHECK_EQUAL(cv.getLevel(fx.getTimeSeries(), expectedTransformedValue, index), fx.getTimeSeries().getValues().at(index-1));
    }


    BOOST_AUTO_TEST_CASE(ConfigVariable_simpleReturns_happyPath, *utf::tolerance(tol))
    {
        const Fixture fx = Fixture();
        const std::string rawConfigVariable = "US_GDP|R|0";

        const Common::ConfigVariable cv = Common::ConfigVariable(rawConfigVariable);

        BOOST_CHECK_EQUAL(cv.getBasename(), fx.getTimeSeries().getName());
        BOOST_CHECK_EQUAL(cv.getTransformationTypeCode(), "R");
        BOOST_CHECK_EQUAL(cv.getLagDependency(), 0);

        const unsigned int index = 2;
        const double expectedTransformedValue = fx.getTimeSeries().getValues().at(index) / fx.getTimeSeries().getValues().at(index-1) - 1;
        BOOST_TEST(cv.getTransformedVariableValue(fx.getTimeSeries(), index) == expectedTransformedValue);
        BOOST_TEST(cv.getLevel(fx.getTimeSeries(), expectedTransformedValue, index) == fx.getTimeSeries().getValues().at(index));
    }

    BOOST_AUTO_TEST_CASE(ConfigVariable_LogReturns_happyPath, *utf::tolerance(tol))
    {
        const Fixture fx = Fixture();
        const std::string rawConfigVariable = "US_GDP|LR|2";

        const Common::ConfigVariable cv = Common::ConfigVariable(rawConfigVariable);

        BOOST_CHECK_EQUAL(cv.getBasename(), fx.getTimeSeries().getName());
        BOOST_CHECK_EQUAL(cv.getTransformationTypeCode(), "LR");
        BOOST_CHECK_EQUAL(cv.getLagDependency(), 2);

        const unsigned int index = 3;
        const double expectedTransformedValue = log(fx.getTimeSeries().getValues().at(index - 2)) - log(fx.getTimeSeries().getValues().at(index - 3));
        BOOST_TEST(cv.getTransformedVariableValue(fx.getTimeSeries(), index) == expectedTransformedValue);
        BOOST_TEST(cv.getLevel(fx.getTimeSeries(), expectedTransformedValue, index) == fx.getTimeSeries().getValues().at(index - 2));
    }
BOOST_AUTO_TEST_SUITE_END()
#pragma clang diagnostic pop