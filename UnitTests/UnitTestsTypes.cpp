//
// Created by Alberto Campi on 22/07/2019.
//
#define BOOST_TEST_MODULE UnitTestsTypes
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
//#include "../wildcatSTKCoreIncludes.h"

#include <iostream>

BOOST_AUTO_TEST_SUITE(emptySuite)
    BOOST_AUTO_TEST_CASE(emptyTestID)
    {
        std::cout << "Empty Test" << std::endl;
    }
BOOST_AUTO_TEST_SUITE_END()