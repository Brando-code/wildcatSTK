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
        Fixture() : f_variableName("US_GDP"), f_values({234.0, 346.0, 447.0, 531.0}), f_ts(f_variableName, f_values, f_dates),
                    f_dates({boost::gregorian::from_string("2017/03/31"), boost::gregorian::from_string("2017/06/30"),
                             boost::gregorian::from_string("2017/09/30"), boost::gregorian::from_string("2017/12/31")}) {}
        ~Fixture() = default;

        Common::TimeSeries getTimeSeries() const { return f_ts;};

        const std::vector<boost::gregorian::date> f_dates;
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
        BOOST_CHECK_EQUAL(cv.getTransformedValue(fx.getTimeSeries(), index), expectedTransformedValue);
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
        BOOST_TEST(cv.getTransformedValue(fx.getTimeSeries(), index) == expectedTransformedValue);
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
        BOOST_TEST(cv.getTransformedValue(fx.getTimeSeries(), index) == expectedTransformedValue);
        BOOST_TEST(cv.getLevel(fx.getTimeSeries(), expectedTransformedValue, index) == fx.getTimeSeries().getValues().at(index - 2));
    }

    BOOST_AUTO_TEST_CASE(ConfigVariable_Levels_happyPath, *utf::tolerance(tol))
    {
        const Fixture fx = Fixture();
        const std::string rawConfigVariable = "US_GDP|L|2";

        const Common::ConfigVariable cv = Common::ConfigVariable(rawConfigVariable);

        BOOST_CHECK_EQUAL(cv.getBasename(), fx.getTimeSeries().getName());
        BOOST_CHECK_EQUAL(cv.getTransformationTypeCode(), "L");
        BOOST_CHECK_EQUAL(cv.getLagDependency(), 2);

        const unsigned int index = 3;
        const double expectedTransformedValue = fx.getTimeSeries().getValues().at(index - 2);
        BOOST_TEST(cv.getTransformedValue(fx.getTimeSeries(), index) == expectedTransformedValue);
        BOOST_TEST(cv.getLevel(fx.getTimeSeries(), expectedTransformedValue, index) == fx.getTimeSeries().getValues().at(index - 2));
    }

    BOOST_AUTO_TEST_CASE(ConfigVariable_badTransformationCode)
    {
        const Fixture fx = Fixture();
        const std::string rawConfigVariable = "US_GDP|X|2";
        BOOST_CHECK_THROW(Common::ConfigVariable cv(rawConfigVariable), std::out_of_range);
    }

    BOOST_AUTO_TEST_CASE(ConfigVariable_badLagDependency)
    {
        const Fixture fx = Fixture();
        const std::string rawConfigVariable = "US_GDP|LR|-100";
        const Common::ConfigVariable cv = Common::ConfigVariable(rawConfigVariable);
        BOOST_CHECK_THROW(cv.getLagDependency(), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(TimeSeries_dates_all)
    {
        const Fixture fx = Fixture();
        const boost::gregorian::date inRangeDate = boost::gregorian::date(2017, 03, 31);
        const boost::gregorian::date outRangeDate = boost::gregorian::date(2019, 03, 31);

        const unsigned int expectedIndex = 0;
        BOOST_CHECK_EQUAL(fx.getTimeSeries().getIndex(inRangeDate), expectedIndex);
        BOOST_CHECK_EQUAL(fx.getTimeSeries().getValue(inRangeDate), fx.getTimeSeries().getValue(expectedIndex));
        BOOST_CHECK_THROW(fx.getTimeSeries().getIndex(outRangeDate), std::out_of_range);
    }
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(DataSet)
    BOOST_AUTO_TEST_CASE(DataSet_all)
    {
        const std::string variableName = "US_GDP";
        const std::string otherVariableName = "UK_CPI";

        const std::vector<double> values = {234.0, 346.0, 447.0, 531.0};
        const std::vector<double> otherValues = {100.0, 98.0, 101.0, 105.45, 104.567};

        const std::vector<boost::gregorian::date> dates = {boost::gregorian::from_string("2017/03/31"), boost::gregorian::from_string("2017/06/30"),
                                                           boost::gregorian::from_string("2017/09/30"), boost::gregorian::from_string("2017/12/31")};
        const std::vector<boost::gregorian::date> otherDates = {boost::gregorian::from_string("2017/03/31"), boost::gregorian::from_string("2017/06/30"),
                                                                boost::gregorian::from_string("2017/09/30"), boost::gregorian::from_string("2017/12/31"),
                                                                boost::gregorian::from_string("2018/03/31")};

        const Common::TimeSeries ts(variableName, values, dates), otherTs(otherVariableName, otherValues, otherDates);

        Common::DataSet ds;
        ds.addData(ts), ds.addData(otherTs);

        const boost::gregorian::date inRangeDate = boost::gregorian::date(2017, 12, 31);
        const boost::gregorian::date otherInRangeDate = boost::gregorian::date(2018, 03, 31);

        BOOST_CHECK_EQUAL(ds.getValue(variableName, inRangeDate), values[3]);
        BOOST_CHECK_EQUAL(ds.getValue(otherVariableName, otherInRangeDate), otherValues[4]);

        ds.removeData(otherVariableName);
        BOOST_CHECK_EQUAL(ds.getData().size(), 1);
        BOOST_CHECK(ds.getData().at(variableName) == ts);
    }
BOOST_AUTO_TEST_SUITE_END()

#pragma clang diagnostic pop