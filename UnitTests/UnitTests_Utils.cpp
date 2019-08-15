//
// Created by Alberto Campi on 30/07/2019.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
//

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <iostream>
#include <cmath>
#include "../Common/Utils/IO/JSONParser.h"
#include "../Common/Utils/General/Tools.h"
//#include "../Common/Types/TimeSeries.h"


namespace utf = boost::unit_test;

const std::string inputRelativePath = "../UnitTests/Inputs/";
const std::string actualOutputRelativePath = "../UnitTests/Outputs/Actual/";
//const std::string expectedOutputRelativePath = "../UnitTests/Outputs/Expected/";

BOOST_AUTO_TEST_SUITE(JSONParser)
    const double tol = 1e-10;

    BOOST_AUTO_TEST_CASE(readJSON_happyPath, *utf::tolerance(tol))
    {
        const std::string inFileName = "readJSON_data_test.json";
        const std::string actualOutFileName = "writeJSON_data_test.json";
        const std::string expectedOutFileName = "writeJSON_data_test_expected.json";
        std::ifstream f(inputRelativePath + inFileName);

        Common::JSONParserDecoratorDataSet js;
        js.readJSON(inputRelativePath + inFileName);
        Common::DataSet fromInputJSONds = js.getDataSet();
        BOOST_CHECK_EQUAL(fromInputJSONds.getData().size(), 4);
        
        js.writeJSON(actualOutputRelativePath + actualOutFileName, fromInputJSONds);
        js.readJSON(actualOutputRelativePath + actualOutFileName);
        Common::DataSet fromOutputJSONds = js.getDataSet();
        BOOST_CHECK(fromInputJSONds == fromOutputJSONds);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Tools)
    BOOST_AUTO_TEST_CASE(getTenorInYearsFromVariableName_all)
    {
        std::string rateVariableName = "US_GOV_PAR_RATE_10Y";
        double expectedTenor = 10.0;
        BOOST_CHECK_EQUAL(Common::getTenorInYearsFromVariableName(rateVariableName), expectedTenor);

        rateVariableName = "US_SWAP_6M";
        expectedTenor = 0.5;
        BOOST_CHECK_EQUAL(Common::getTenorInYearsFromVariableName(rateVariableName), expectedTenor);

        rateVariableName = "SONIA_ON";
        expectedTenor = 1./365.;
        BOOST_CHECK_EQUAL(Common::getTenorInYearsFromVariableName(rateVariableName), expectedTenor);

        rateVariableName = "SONIA_7D";
        expectedTenor = 7./365.;
        BOOST_CHECK_EQUAL(Common::getTenorInYearsFromVariableName(rateVariableName), expectedTenor);

        rateVariableName = "SONIA_46U";
        BOOST_CHECK_THROW(Common::getTenorInYearsFromVariableName(rateVariableName), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END()

//#pragma clang diagnostic pop