cmake_minimum_required(VERSION 3.12)

project(OSM2Population VERSION 0.1
    DESCRIPTION "Program to compute the distribution of population within an area based on OSM extracted datas."
    LANGUAGES CXX)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

#################### Options ####################
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeBuildTypeUtils.cmake)

option(WARNINGS "" OFF)
option(COMPILE_FOR_NATIVE "" OFF)
option(COMPILE_WITH_LTO "" OFF)

#################### Packages ####################
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(osm2population src/io/parse_geojson.cpp
                        src/osm2population.cpp)
target_include_directories(osm2population PRIVATE include)
target_link_libraries(osm2population CONAN_PKG::boost)
target_link_libraries(osm2population CONAN_PKG::tbb)
target_link_libraries(osm2population CONAN_PKG::simdjson)
target_compile_options_if_exists(osm2population PRIVATE ${WARNINGS} -Wall)
target_compile_options_if_exists(osm2population PRIVATE ${COMPILE_FOR_NATIVE} -march=native)
target_compile_options_if_exists(osm2population PRIVATE ${COMPILE_WITH_LTO} -flto)
