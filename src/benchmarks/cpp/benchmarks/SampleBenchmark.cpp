#include <hayai.hpp>
#include <cmath>

static double result;
static double result2;

BENCHMARK(Math, acosh, 1000, 1000000) {
    result = acosh(3.0);
    result2 = acosh(5.0);
}

