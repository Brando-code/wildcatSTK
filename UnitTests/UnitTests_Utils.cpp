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
#include "../Common/Utils/General/AlgebraicExpressionInterpreter.h"


namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

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

    BOOST_AUTO_TEST_CASE(StringSplitAlgebraic_happyPath)
    {
        std::string expression = "a + b * c / d ^ n + 2.5";
        Common::StringSplitAlgebraicDecorator adc;
        Common::AlgebraicOperatorsGrammar grammar;

        adc.setOperatorsGrammar(grammar);
        adc.splitExpression(expression);
        const std::vector<std::string> expectedComponents = {"a", "b", "c", "d", "n", "2.5"};
        const std::vector<std::string> expectedOperators = {"+", "*", "/", "^", "+"};
        const std::vector<std::string> expectedTokenized = {"a", "+", "b", "*", "c",  "/", "d", "^", "n", "+", "2.5" };
        BOOST_TEST(adc.get() == expectedComponents, tt::per_element());
        BOOST_TEST(adc.getOrderedOperators() == expectedOperators, tt::per_element());
        BOOST_TEST(adc.getTokenized() == expectedTokenized, tt::per_element());
    }

    BOOST_AUTO_TEST_CASE(StringSplitAlgebraic_brackets)
    {
        std::string expression = "a + (b * c / d) ^ n + 2.5";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::StringSplitAlgebraicDecorator adc(grammar);

        adc.splitExpression(expression);
        const std::vector<std::string> expectedComponents = {"a", "b", "c", "d", "n", "2.5"};
        const std::vector<std::string> expectedOperators = {"+", "(", "*", "/", ")", "^", "+"};
        const std::vector<std::string> expectedTokenized = {"a", "+", "(", "b", "*", "c",  "/", "d", ")", "^", "n", "+", "2.5" };
        BOOST_TEST(adc.get() == expectedComponents, tt::per_element());
        BOOST_TEST(adc.getOrderedOperators() == expectedOperators, tt::per_element());
        BOOST_TEST(adc.getTokenized() == expectedTokenized, tt::per_element());
    }

    BOOST_AUTO_TEST_CASE(StringSplitAlgebraic_invalid)
    {
        std::string expression = "+ a + b * * c / d ^ n +";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::StringSplitAlgebraicDecorator adc(grammar);

        BOOST_CHECK_THROW(adc.splitExpression(expression), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(StringSplitAlgebraic_invalid_brackets)
    {
        std::string expression = "(a + b) * ( c / d ^ n +";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::StringSplitAlgebraicDecorator adc(grammar);

        BOOST_CHECK_THROW(adc.splitExpression(expression), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(StringSplitAlgebraic_invalid_nested_brackets)
    {
        std::string expression = "(((a) + b) * ( c )/ d ^ n +";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::StringSplitAlgebraicDecorator adc(grammar);

        BOOST_CHECK_THROW(adc.splitExpression(expression), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(StringSplitAlgebraic_invalid_bracket_operator)
    {
        std::string expression = "(a + b) c / d ^ n +";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::StringSplitAlgebraicDecorator adc(grammar);;

        BOOST_CHECK_THROW(adc.splitExpression(expression), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(EvaluateStringExpression_happyPath)
    {
        std::string expression = "a + b * c / d ^ n + 2.5";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::AlgebraicExpressionParser aep(expression, grammar);

        std::unordered_map<std::string, double> kvp = {{"a", 1}, {"b", 2}, {"c", 6},
                                                       {"d", 2}, {"n", 3}};
        Common::AlgebraicExpressionContext ctx(kvp);

        const double expected = 1. + 2. * 6./pow(2, 3) + 2.5;
        const double actual = aep.evaluate(ctx);
        BOOST_CHECK_EQUAL(actual, expected);
    }

    BOOST_AUTO_TEST_CASE(EvaluateStringExpression_power_happyPath)
    {
        std::string expression = "a^n^m - b * c * d * 2.5";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::AlgebraicExpressionParser aep(expression, grammar);

        std::unordered_map<std::string, double> kvp = {{"a", 3}, {"b", 2}, {"c", 6},
                                                       {"d", 2}, {"n", 2}, {"m", 3}};
        Common::AlgebraicExpressionContext ctx(kvp);

        const double expected = pow(3, pow(2, 3)) - 2 * 6 * 2 * 2.5;
        const double actual = aep.evaluate(ctx);
        BOOST_CHECK_EQUAL(actual, expected);
    }

    BOOST_AUTO_TEST_CASE(EvaluateStringExpression_invalid)
    {
        std::string expression = "*a + b * + c / d ^ n + 2.5";
        Common::AlgebraicOperatorsGrammar grammar;
        Common::AlgebraicExpressionParser aep(expression, grammar);

        std::unordered_map<std::string, double> kvp = {{"a", 1}, {"b", 2}, {"c", 6},
                                                       {"d", 2}, {"n", 3}};
        Common::AlgebraicExpressionContext ctx(kvp);

        BOOST_CHECK_THROW(aep.evaluate(ctx), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END()

//#pragma clang diagnostic pop