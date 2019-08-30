//
// Created by Alberto Campi on 2019-08-30.
//
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "Utils.h"
#include "../Common/Types/DataSet.h"
#include "../Common/Utils/IO/JSONParser.h"
#include "../Common/Auxiliary/FormulaVariable.h"
#include "../Common/Seasonality/SeasonalDecompose.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

const std::string expectedOutputRelativePath = "../UnitTests/Outputs/Expected/";
const std::string actualOutputRelativePath = "../UnitTests/Outputs/Actual/";
const std::string inputRelativePath = "../UnitTests/Inputs/";

BOOST_AUTO_TEST_SUITE(FormulaVariable)

    void loadDataSet(const std::string& JSONfilePath, Common::DataSet& ds)
    {
        ds.clearAllData();
        Common::JSONParserDecoratorDataSet js;
        js.readJSON(JSONfilePath);
        ds = js.getDataSet();
    }

    const std::string fileName = "sample_dataSet_clean.json";
    BOOST_AUTO_TEST_CASE(AlgebraicSpread_happyPath)
    {
        const std::string spreadExpression = "US_TSY_20Y - US_TBILL_3M";
        const Common::AlgebraicOperatorsGrammar grammar;
        boost::gregorian::date d(2019, 3, 31);

        Common::DataSet ds;
        loadDataSet(inputRelativePath + fileName, ds);
        const double expectedSpreadValue = ds.getValue("US_TSY_20Y", d) - ds.getValue("US_TBILL_3M", d);

        Common::FormulaVariableAlgebraic fva(spreadExpression, grammar);
        BOOST_CHECK_EQUAL(fva.evaluate(ds, d), expectedSpreadValue);
    }

    BOOST_AUTO_TEST_CASE(FunctionalDeSeason_happyPath)
    {
        Common::DataSet ds;
        loadDataSet(inputRelativePath + fileName, ds);

        std::string variable = "RPI_FOOD_DRINK_TOBACCO_NSA";
        std::string decompositionType = "additive";
        const unsigned int period = 4;
        const Common::FormulaVariableFunctionalDeSeason fvfd(variable, decompositionType, period);

        Common::SeasonalDecomposeConvolutionAdditive sdcd(period);
        sdcd.decompose(ds.getTimeSeries(variable));

        boost::gregorian::date d(2017, 3, 31);
        double expectedDeSeasonedValue = (sdcd.getTrend() + sdcd.getNoise()).getValue(d);
        BOOST_CHECK_EQUAL(fvfd.evaluate(ds, d), expectedDeSeasonedValue);

        decompositionType = "multiplicative";
        variable = "OV_UK_VISITORS_NSA";
        const Common::FormulaVariableFunctionalDeSeason otherFvfd(variable, decompositionType, period);

        Common::SeasonalDecomposeConvolutionMultiplicative sdcm(period);
        sdcm.decompose(ds.getTimeSeries(variable));
        expectedDeSeasonedValue = (sdcm.getTrend() + sdcm.getNoise()).getValue(d);

        BOOST_CHECK_EQUAL(otherFvfd.evaluate(ds, d), expectedDeSeasonedValue);
    }

BOOST_AUTO_TEST_SUITE_END()
