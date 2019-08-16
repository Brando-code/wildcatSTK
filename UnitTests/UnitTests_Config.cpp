//
// Created by Alberto Campi on 22/07/2019.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
//
#define BOOST_TEST_MODULE UnitTests
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <iostream>
#include <cmath>
#include "Utils.h"
#include "../Common/Config/ConfigVariable.h"
#include "../Common/Config/ConfigModelSpec.h"
#include "../Common/Config/CurveModelDef.h"
#include "../Common/Types/TimeSeries.h"
#include "../Common/Types/DataSet.h"
#include "../Common/Types/ConfigMap.h"
#include "../Common/Utils/IO/JSONParser.h"
#include "../Common/Math/Relative/RelativeModel.h"
#include "../Common/Math/MLRegression/RegressionModel.h"
#include "../Common/Math/Interpolation/Interpolator.h"


namespace utf = boost::unit_test;

const std::string inputRelativePath = "../UnitTests/Inputs/";

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

        //Test copy semantics
        Common::ConfigVariable ccv = cv;
        BOOST_CHECK(cv == ccv);
        Common::ConfigVariable cccv("UK_HPI|LR|0");
        BOOST_CHECK(cccv != cv);
        cccv = cv;
        BOOST_CHECK(cccv == cv);

        //Test move semantics
        Common::ConfigVariable mcv(std::move(ccv)) ;
        BOOST_CHECK(cv == mcv);
        mcv = Common::ConfigVariable(cv);
        BOOST_CHECK(cv == mcv);

        //Test other methods
        BOOST_CHECK_EQUAL(cv.getBasename(), fx.getTimeSeries().getName());
        BOOST_CHECK_EQUAL(cv.getTransformationTypeCode(), "D");
        BOOST_CHECK_EQUAL(cv.getLagDependency(), 1);

        const unsigned int index = 2;
        const double expectedTransformedValue = fx.getTimeSeries().getValues().at(index-1) - fx.getTimeSeries().getValues().at(index-2);
        BOOST_CHECK_EQUAL(cv.getTransformedValue(fx.getTimeSeries(), index), expectedTransformedValue);
        BOOST_CHECK_EQUAL(cv.getLevel(fx.getTimeSeries(), expectedTransformedValue, index), fx.getTimeSeries().getValues().at(index-1));

        const std::vector<double> expectedTransformedValues = {fx.getTimeSeries().getValue(index - 1) - fx.getTimeSeries().getValue(index - 2),
                                                               fx.getTimeSeries().getValue(index) - fx.getTimeSeries().getValue(index - 1)};
        BOOST_TEST(cv.getTransformedTimeSeriesValues(fx.getTimeSeries()) == expectedTransformedValues);

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

        const std::vector<double> expectedTransformedValues = {fx.getTimeSeries().getValue(index - 1) / fx.getTimeSeries().getValue(index - 2) - 1,
                                                               fx.getTimeSeries().getValue(index) / fx.getTimeSeries().getValue(index - 1) - 1,
                                                               fx.getTimeSeries().getValue(index + 1) / fx.getTimeSeries().getValue(index) - 1};
        BOOST_TEST(cv.getTransformedTimeSeriesValues(fx.getTimeSeries()) == expectedTransformedValues);
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

        const std::vector<double> expectedTransformedValues = {log(fx.getTimeSeries().getValue(1)) - log(fx.getTimeSeries().getValue(0))};
        BOOST_TEST(cv.getTransformedTimeSeriesValues(fx.getTimeSeries()) == expectedTransformedValues);
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

        Common::DataSet otherDs(std::vector<Common::TimeSeries>{ts, otherTs});
        BOOST_CHECK(ds == otherDs);

        const boost::gregorian::date inRangeDate = boost::gregorian::date(2017, 12, 31);
        const boost::gregorian::date otherInRangeDate = boost::gregorian::date(2018, 03, 31);

        BOOST_CHECK_EQUAL(ds.getValue(variableName, inRangeDate), values[3]);
        BOOST_CHECK_EQUAL(ds.getValue(otherVariableName, otherInRangeDate), otherValues[4]);
        BOOST_CHECK_THROW(ds.getValue("US_CPI", otherInRangeDate), std::runtime_error);

        ds.removeData(otherVariableName);
        BOOST_CHECK_EQUAL(ds.getData().size(), 1);
        BOOST_CHECK(ds != otherDs);
        BOOST_CHECK(ds.getData().at(variableName) == ts);
        BOOST_CHECK(ds.getTimeSeries(variableName) == ts);
        BOOST_CHECK_THROW(ds.getTimeSeries("US_CPI"), std::runtime_error);
    }
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(ConfigModelSpec)
    struct Fixture
    {
        Fixture() : f_dv("DOW_JONES|R|0"), f_ivs({Common::ConfigVariable("US_GDP|R|0"),
                                                  Common::ConfigVariable("WTI|R|1"),
                                                  Common::ConfigVariable("UK_CPI|R|2")}),
                    f_m(), f_modelSubType(), f_startDate() {}


        const Common::ConfigVariable f_dv;
        const std::vector<Common::ConfigVariable> f_ivs;
        int f_m;
        std::string f_modelSubType;
        boost::gregorian::date f_startDate;

    };

    void loadDataSet(const std::string& fullFileName, Common::DataSet& ds)
    {
        Common::JSONParserDecoratorDataSet js;
        js.readJSON(fullFileName);
        ds = js.getDataSet();
    }

    const std::string inputDataSetFileName = "readJSON_data_test.json";

    BOOST_AUTO_TEST_CASE(ConfigModelSpec_badIdVariables)
    {
        Fixture fx;
        fx.f_modelSubType = "growth";
        fx.f_m = 1;

        BOOST_CHECK_THROW(Common::ConfigModelSpecRelative cms(fx.f_dv, std::vector<Common::ConfigVariable>(), fx.f_modelSubType, fx.f_m),
                std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(ConfigModelSpec_relative_growth_happyPath)
    {
        Fixture fx;
        fx.f_modelSubType = "growth";
        fx.f_m = 1;

        Common::ConfigModelSpecRelative cms(fx.f_dv, fx.f_ivs, fx.f_modelSubType, fx.f_m);
        BOOST_CHECK(cms.getDependentVariable() == fx.f_dv);
        BOOST_CHECK(cms.getIndependentVariables() == fx.f_ivs);

        //Testing copy semantics
        Common::ConfigModelSpecRelative ccms(cms);
        BOOST_CHECK(cms == ccms);
        ccms = cms;
        BOOST_CHECK(cms == ccms);

        //Testing move semantics
        Common::ConfigModelSpecRelative mcms(std::move(ccms));
        BOOST_CHECK(cms == mcms);
        mcms = Common::ConfigModelSpecRelative(cms);
        BOOST_CHECK(cms == mcms);

        Common::DataSet ds;
        loadDataSet(inputRelativePath + inputDataSetFileName, ds);

        Common::ConfigModelSpecRelative spec(fx.f_dv, fx.f_ivs, fx.f_modelSubType, fx.f_m);
        const unsigned int index = ds.getTimeSeries(fx.f_ivs.at(0).getBasename()).getValues().size() - 1;
        const double ivTransformedValue = fx.f_ivs.at(0).getTransformedValue(ds.getTimeSeries(fx.f_ivs.at(0).getBasename()), index);
        const double dvExpectedValue = ds.getValue(fx.f_dv.getBasename(), index - 1) * (1 + ivTransformedValue);

        spec.calibrate(ds);
        BOOST_CHECK_EQUAL(spec.predict(ds, ds.getTimeSeries(fx.f_dv.getBasename()).getValues().size() - 1), dvExpectedValue);
        BOOST_CHECK_EQUAL(spec.getModelSubType(), fx.f_modelSubType);
        BOOST_CHECK_EQUAL(spec.getMultiplier(), fx.f_m);
    }

    BOOST_AUTO_TEST_CASE(ConfigModelSpec_regression_startDate_happyPath)
    {
        Fixture fx;
        fx.f_startDate = boost::gregorian::date(2007, 03, 31);
        Common::ConfigModelSpecRegression spec(fx.f_dv, fx.f_ivs, fx.f_modelSubType, fx.f_startDate);

        boost::gregorian::date expectedFirstValidRegressionDate(2007, 9, 30);

        Common::DataSet ds;
        loadDataSet(inputRelativePath + inputDataSetFileName, ds);

        BOOST_CHECK_EQUAL(spec.getFirstValidRegressionDate(ds), expectedFirstValidRegressionDate);
        BOOST_CHECK_EQUAL(spec.getModelSubType(), fx.f_modelSubType);

        fx.f_startDate = boost::gregorian::date(2008, 3, 31);
        Common::ConfigModelSpecRegression anotherSpec(fx.f_dv, fx.f_ivs, fx.f_modelSubType, fx.f_startDate);
        BOOST_CHECK_EQUAL(anotherSpec.getFirstValidRegressionDate(ds), fx.f_startDate);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(ConfigMap)
    struct Fixture
    {
        //Default fixture
        Fixture() : f_dv("DOW_JONES|R|0"), f_ivs({Common::ConfigVariable("US_GDP|R|0"),
                                                  Common::ConfigVariable("WTI|R|1"),
                                                  Common::ConfigVariable("UK_CPI|R|2")}),
                    f_m(), f_modelSubType(), f_startDate() {}

        //Fixture with parameters
        Fixture(const std::string rawDepVarName, std::initializer_list<Common::ConfigVariable> rawIDepVariables) :
                f_dv(rawDepVarName), f_ivs({rawIDepVariables}), f_m(), f_modelSubType(), f_startDate() {}


        const Common::ConfigVariable f_dv;
        const std::vector<Common::ConfigVariable> f_ivs;
        int f_m;
        std::string f_modelSubType;
        boost::gregorian::date f_startDate;

    };

    BOOST_AUTO_TEST_CASE(ConfigMap_configModelSpec_all)
    {
        Fixture defFx;
        defFx.f_modelSubType = "ols_lm";
        defFx.f_startDate = boost::gregorian::from_string("2005/09/10");

        Fixture otherFx("UK_HPI|L|0", {Common::ConfigVariable("UK_UNEMP|D|1"), Common::ConfigVariable("UK_CPI|R|0")});
        otherFx.f_modelSubType = "garch";
        otherFx.f_startDate = boost::gregorian::from_string("1987/12/31");

        Common::ConfigModelSpecRegression spec(defFx.f_dv, defFx.f_ivs, defFx.f_modelSubType, defFx.f_startDate);
        Common::ConfigModelSpecRegression otherSpec(otherFx.f_dv, otherFx.f_ivs, otherFx.f_modelSubType, otherFx.f_startDate);

        //Test constructors
        Common::ConfigMap<Common::ConfigModelSpec> table;
        table.addConfigItem(spec.getDependentVariable().getBasename(), spec);
        table.addConfigItem(otherSpec.getDependentVariable().getBasename(), otherSpec);

        Common::ConfigMap<Common::ConfigModelSpec> otherTable;

        //Test copy semantics
        Common::ConfigMap<Common::ConfigModelSpec> cTable(table);
        BOOST_CHECK(table == cTable);
        otherTable = cTable;
        BOOST_CHECK(cTable == table);

        //Test move semantics
        Common::ConfigMap<Common::ConfigModelSpec> mvTable(std::move(cTable));
        BOOST_CHECK(table == mvTable);
        mvTable = Common::ConfigMap<Common::ConfigModelSpec>(table);
        BOOST_CHECK(table == mvTable);

        BOOST_CHECK_EQUAL(table.getConfigMap().size(), 2);
        BOOST_CHECK(*table.getValue(otherFx.f_dv.getBasename()) == otherSpec);

        table.removeConfigItem(otherFx.f_dv.getBasename());
        BOOST_CHECK_THROW(*table.getValue(otherFx.f_dv.getBasename()), std::out_of_range);

        table.clearAllConfigItems();
        BOOST_CHECK_EQUAL(table.getConfigMap().size(), 0);
        BOOST_CHECK(table != otherTable);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(curveModel)
    struct Fixture
    {
        Fixture() : f_tenors({"US_TBILL_3M", "US_TBILL_6M", "US_TSY_1Y", "US_TSY_5Y", "US_TSY_10Y", "US_TSY_20Y"}),
                    f_curveName("us_govi_curve"), f_interpolation("linear") {}

        std::vector<std::string> f_tenors;
        std::string f_curveName;
        std::string f_interpolation;
    };

    void loadDataSet(const std::string& fileName, Common::DataSet& ds)
    {
        ds.clearAllData();
        Common::JSONParserDecoratorDataSet js;
        js.readJSON(fileName);
        ds = js.getDataSet();
    }

    BOOST_AUTO_TEST_CASE(yieldCurve)
    {
        Common::CurveModelDef cmd(Fixture().f_curveName, Fixture().f_interpolation, Fixture().f_tenors);
        boost::gregorian::date d(2010, 6, 30);

        Common::DataSet ds;
        loadDataSet(inputRelativePath + "curveModelDef_data_test.json", ds);

        Common::YieldCurve expectedCurve;
        for (const auto& it: Fixture().f_tenors)
        {
            const double t = Common::getTenorInYearsFromVariableName(it), val = ds.getValue(it, d);
            expectedCurve.emplace(t, val);
        }

        Common::YieldCurve actualCurve = cmd.getYieldCurve(ds, d);
        BOOST_CHECK_EQUAL_COLLECTIONS(actualCurve.begin(), actualCurve.end(),
                expectedCurve.begin(), expectedCurve.end());
        BOOST_CHECK_EQUAL(cmd.getCurveName(), Fixture().f_curveName);
        
        //Common::CurveModelDef mcmd(Common::CurveModelDef(Fixture().f_curveName, Fixture().f_interpolation, Fixture().f_tenors));
        //Test copy semantics
        Common::CurveModelDef ccmd(cmd);
        BOOST_CHECK(ccmd == cmd);
        ccmd = cmd;
        BOOST_CHECK(ccmd == cmd);
        
        //Test move semantics
        Common::CurveModelDef mcmd(std::move(ccmd));
        BOOST_CHECK(mcmd == cmd);
        mcmd = Common::CurveModelDef(cmd);
        BOOST_CHECK(mcmd == cmd);
    }

    BOOST_AUTO_TEST_CASE(curveModelSpec_linear)
    {
        Fixture fx;
        Common::ConfigMap<Common::CurveModelDef> m;
        m.addConfigItem(Fixture().f_curveName, Common::CurveModelDef(Fixture().f_curveName, Fixture().f_interpolation, Fixture().f_tenors));

        const Common::ConfigVariable variable("US_TSY_7Y|L|0");

        boost::gregorian::date d(2010, 6, 30);

        Common::DataSet ds;
        loadDataSet(inputRelativePath + "curveModelDef_data_test.json", ds);

        const double r = m.getValue(Fixture().f_curveName) -> interpolate(variable, d, ds);
        std::cerr << r << std::endl;
    }

BOOST_AUTO_TEST_SUITE_END()

//#pragma clang diagnostic pop