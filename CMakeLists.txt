cmake_minimum_required(VERSION 2.8.4)
project(chroma2Cpp)

add_subdirectory(lib/hayai)
INCLUDE_DIRECTORIES(src/benchmarks/headers)
include_directories(lib/hayai/src)


SET(SOURCE_FILES
    src/benchmarks/cpp/Triangle.cpp
    src/benchmarks/cpp/Hitpoint.cpp
    src/benchmarks/cpp/Ray.cpp
    src/benchmarks/cpp/Fibonacci.cpp
    src/benchmarks/cpp/benchmarks/main.cpp
    src/benchmarks/cpp/benchmarks/SampleBenchmark.cpp
    src/benchmarks/cpp/benchmarks/FibonacciBenchmark.cpp
    src/benchmarks/cpp/benchmarks/MoellerTrumboreTriangleIntersectionTest.cpp
)

ADD_EXECUTABLE(benchmarks ${SOURCE_FILES})
target_link_libraries(benchmarks hayai_main ${LIB_TIMING})

