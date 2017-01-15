#ifndef ACCUMULATIONBUFFER_H_
#define ACCUMULATIONBUFFER_H_

#include <cassert>
#include <cstring>

#include "Image.h"


class AccumulationBuffer : public Image {

public:

    int acc;
    //int* accArray;

    AccumulationBuffer();

    AccumulationBuffer(int, int);

    ~AccumulationBuffer();


    inline void accumulateGlobal(const Vector3 &value, int pos) {
        pixels[pos] = (pixels[pos] * acc + value) / (acc + 1);
    }


    inline void clear() {
        //memset(accArray,0,width*height*4);
        memset(pixels, 0, width * height * sizeof(Vector3));
        acc = 0;

    }


    inline void flush() {
        //memset(accArray,0,width*height*4);
        acc = 0;
    }


    inline void operator+=(const Image &acc2) {

        assert(width == acc2.width && height == acc2.height);
        for (int i = 0; i < width * height; i++) {
            accumulateGlobal(acc2[i], i);
        }
    }


    inline void merge(const Image &acc2) {

        for (int i = 0; i < width * height; i++)
            pixels[i] += acc2.pixels[i];
    }
};

#endif /*ACCUMULATIONBUFFER_H_*/
