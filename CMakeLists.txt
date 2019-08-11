cmake_minimum_required(VERSION 3.10)
project(wildcatSTKCore)
enable_testing()

set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.62.0 REQUIRED COMPONENTS unit_test_framework date_time)

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(${Boost_INCLUDE_DIR})

add_executable(UTests UnitTests/UnitTests_Config.cpp UnitTests/UnitTests_Utils.cpp wildcatSTKCoreIncludes.h UnitTests/UnitTests_Math.cpp UnitTests/Utils.h)
target_include_directories(UTests PRIVATE ${BOOST_INCLUDE_DIRS})

target_link_libraries(UTests wildcatSTKCore ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_custom_command(TARGET UTests PRE_BUILD COMMAND lcov --directory . --zerocounters)

add_library(wildcatSTKCore Common/Config/ConfigVariable.cpp Common/Config/ConfigVariable.h Common/Types/TimeSeries.cpp
        Common/Types/TimeSeries.h Global/Mappings/FactoryMappings.cpp Global/Mappings/FactoryMappings.h
        Common/Utils/General/Tools.cpp Common/Utils/General/Tools.h Common/Types/DataSet.cpp Common/Types/DataSet.h Common/Config/ConfigModelSpec.cpp
        Common/Config/ConfigModelSpec.h Common/Math/Relative/RelativeModel.cpp Common/Math/Relative/RelativeModel.h
        Common/Utils/IO/JSONParser.cpp Common/Utils/IO/JSONParser.h Common/Utils/IO/CSVParser.cpp Common/Utils/IO/CSVParser.h
        Common/Math/Statistics/Stat.cpp Common/Math/Statistics/Stat.h Common/Math/MLRegression/RegressionModel.cpp
        Common/Math/MLRegression/RegressionModel.h Common/Types/ConfigModelSpecTable.cpp Common/Types/ConfigModelSpecTable.h Common/Concepts.h
        Common/Config/CurveModelDef.cpp Common/Config/CurveModelDef.h Common/Math/Interpolation/Interpolator.cpp Common/Math/Interpolation/Interpolator.h
        Common/Math/Interpolation/LinearInterpolator.cpp Common/Math/Interpolation/LinearInterpolator.h
        Common/Math/Interpolation/NaturalCubicSplineInterpolator.cpp Common/Math/Interpolation/NaturalCubicSplineInterpolator.h)

target_link_libraries(wildcatSTKCore ${Boost_LIBRARIES})