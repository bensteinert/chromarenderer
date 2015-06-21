cmake_minimum_required(VERSION 2.8.4)
project(chroma2Cpp)

INCLUDE_DIRECTORIES(src/chroma/headers)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
SET(SOURCE_FILES
    src/chroma/cpp/Sampler.cpp
    src/chroma/cpp/Material.cpp
    src/chroma/cpp/Primitive.cpp
    src/chroma/cpp/Triangle.cpp
    src/chroma/cpp/Sphere.cpp
    src/chroma/cpp/Hitpoint.cpp
    src/chroma/cpp/Ray.cpp
    src/chroma/cpp/BVHNode.cpp
    src/chroma/cpp/BVHTree.cpp
    src/chroma/cpp/Scene.cpp
    src/chroma/cpp/SunSky.cpp
    src/chroma/cpp/NextEventEstimatorLight.cpp
    src/chroma/cpp/Fibonacci.cpp
    src/chroma/cpp/Fibonacci.cpp
    )

ADD_LIBRARY(chroma2-lib ${SOURCE_FILES})
ADD_EXECUTABLE (chroma2 ${SOURCE_FILES} src/chroma/cpp/main.cpp)
