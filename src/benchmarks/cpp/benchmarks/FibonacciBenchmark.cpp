#include <hayai.hpp>
#include <Fibonacci.h>

Fibonacci fibonacci = Fibonacci();

static long result;

BENCHMARK(Fibonacci, Fib_100, 100, 1000000) {
    result = fibonacci.compute(100);
}

