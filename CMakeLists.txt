cmake_minimum_required(VERSION 2.8.4)
project(chroma2Cpp)

INCLUDE_DIRECTORIES(src/chroma/headers)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
SET(SOURCE_FILES src/chroma/cpp/Triangle.cpp src/chroma/cpp/Hitpoint.cpp src/chroma/cpp/Ray.cpp)

ADD_LIBRARY(chroma2 ${SOURCE_FILES})
