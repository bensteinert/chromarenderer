#ifndef IMAGE_H_
#define IMAGE_H_

#include "Vector3.h"

class Image {

public:
    int width;
    int height;
    Vector3 *pixels;

    Image();

    ~Image();

    Image(int, int);

    Image(const char path[]);

    Image &toneMap();

    bool saveHDR(const char path[]) const;

    bool saveTonemapped(const char path[]);

    float L1Norm() const;

    Vector3 L1NormRGB() const;

    void convertXYZtoRGB(Image &dest_img);

    void clear();


    friend inline Image *operator+(const Image &lhs, const Image &rhs) {
        if (lhs.height != rhs.height || lhs.width != rhs.width) {
            std::cerr << "image sizes are not the same!" << std::endl;
            return 0;
        }

        Image *result = new Image(lhs.width, lhs.height);

        for (int i = 0; i < lhs.width * lhs.height; i++) {
            (*result)[i] = lhs.pixels[i] + rhs.pixels[i];
        }

        return result;
    }


    friend inline Image *operator-(const Image &lhs, const Image &rhs) {
        if (lhs.height != rhs.height || lhs.width != rhs.width) {
            std::cerr << "image sizes are not the same!" << std::endl;
            return 0;
        }

        Image *result = new Image(lhs.width, lhs.height);

        for (int i = 0; i < lhs.width * lhs.height; i++) {
            (*result)[i] = lhs.pixels[i] - rhs.pixels[i];
        }

        return result;
    }

    // pixel access
    inline Vector3 &operator[](unsigned int pos) { return pixels[pos]; }


    inline Vector3 operator[](unsigned int pos) const { return pixels[pos]; }
};

#endif
