cmake_minimum_required(VERSION 2.8.4)
project(chroma2Cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mfpmath=sse")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lSDLmain -lSDL -framework Cocoa" )

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL REQUIRED sdl)
FIND_PACKAGE(Boost 1.36.0 REQUIRED)
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(src/chroma/headers)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
SET(SOURCE_FILES
    src/chroma/cpp/tga.cpp
    src/chroma/cpp/rgbe.cpp
    src/chroma/cpp/Matrix3x3.cpp
    src/chroma/cpp/Sampler.cpp
    src/chroma/cpp/Material.cpp
    src/chroma/cpp/Primitive.cpp
    src/chroma/cpp/Triangle.cpp
    src/chroma/cpp/Sphere.cpp
    src/chroma/cpp/Disk.cpp
    src/chroma/cpp/Hitpoint.cpp
    src/chroma/cpp/Ray.cpp
    src/chroma/cpp/BVHNode.cpp
    src/chroma/cpp/BVHTree.cpp
    src/chroma/cpp/Scene.cpp
    src/chroma/cpp/SunSky.cpp
    src/chroma/cpp/NextEventEstimatorLight.cpp
    src/chroma/cpp/Fibonacci.cpp
    src/chroma/cpp/AbstractRenderCanvas.cpp
    src/chroma/cpp/SDLRenderCanvas.cpp
    src/chroma/cpp/Image.cpp
    src/chroma/cpp/Aperture.cpp
    src/chroma/cpp/Camera.cpp
    src/chroma/cpp/NextEventEstimatorLens.cpp
    src/chroma/cpp/AccumulationBuffer.cpp
    src/chroma/cpp/ThinLensCamera.cpp
    src/chroma/cpp/Renderer.cpp
    src/chroma/cpp/PathTracer.cpp
    )

ADD_LIBRARY(chroma2-lib ${SOURCE_FILES})
ADD_EXECUTABLE (chroma2 ${SOURCE_FILES} src/chroma/cpp/main.cpp)
