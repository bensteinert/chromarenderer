cmake_minimum_required(VERSION 2.8.4)
project(chroma2Cpp)

INCLUDE_DIRECTORIES(src/main/headers)
set(SOURCE_FILES src/main/cpp/Triangle.cpp src/main/cpp/Hitpoint.cpp src/main/cpp/Ray.cpp)
ADD_LIBRARY(chroma2 ${SOURCE_FILES})
