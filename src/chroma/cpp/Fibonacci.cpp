#include "Fibonacci.h"

    unsigned long Fibonacci::compute(unsigned int n) {
        if (n <= 0) {
            return 0;
        } else if (n == 1) {
            return 1;
        } else {
            unsigned long a = 0;
            unsigned long b = 1;
            unsigned long i = 2;
            while (i <= n) {
                unsigned long aa = b;
                unsigned long bb = a + b;
                a = aa;
                b = bb;
                i++;
            }
            return b;
        }
    }
