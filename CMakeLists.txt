cmake_minimum_required(VERSION 3.2)
project(binomial)
include(FindCppUnit.cmake)
include_directories(include)

file(GLOB sources src/*.cpp)
set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
link_libraries(-lpthread)
add_executable(binomial ${sources} main.cpp)

file(GLOB test-sources test/*.cpp)
include_directories(${CPPUNIT_INCLUDE_DIR})
link_libraries(${CPPUNIT_LIBRARIES})
add_executable(binomial-test ${sources} ${test-sources})