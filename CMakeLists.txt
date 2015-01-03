cmake_minimum_required(VERSION 2.8.4)
project(chroma2Cpp)

INCLUDE_DIRECTORIES(src/chroma/headers)
set(SOURCE_FILES src/chroma/cpp/Triangle.cpp src/chroma/cpp/Hitpoint.cpp src/chroma/cpp/Ray.cpp)
ADD_LIBRARY(chroma2 ${SOURCE_FILES})
