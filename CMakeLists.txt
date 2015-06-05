cmake_minimum_required(VERSION 3.2)
project(binomial)

include_directories(include)
file(GLOB sources src/*.cpp)
set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
link_libraries(-lpthread)
add_executable(binomial ${sources})