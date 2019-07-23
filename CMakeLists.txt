cmake_minimum_required(VERSION 3.10)
project(wildcatSTKCore)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
find_package(Boost 1.62.0 REQUIRED COMPONENTS unit_test_framework)

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(${Boost_INCLUDE_DIR})

add_executable(UTests UnitTests/UnitTestsTypes.cpp wildcatSTKCoreIncludes.h)
target_include_directories(UTests PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(UTests wildCatSTKCore ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_custom_command(TARGET UTests PRE_BUILD COMMAND lcov --directory . --zerocounters)

add_library(wildCatSTKCore Common/Common.h Common/Types/ConfigVariable.cpp Common/Types/ConfigVariable.h Common/Types/TimeSeries.cpp
        Common/Types/TimeSeries.h Global/Mappings/FactoryMappings.cpp Global/Mappings/FactoryMappings.h Global/Global.h)