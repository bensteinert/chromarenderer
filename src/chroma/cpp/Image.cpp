#include <cstring>
#include <mm_malloc.h>

#include "Image.h"
#include "Matrix3x3.h"
#include "rgbe.h"
#include "tga.h"
#include "ChromaCIE.h"


Image::Image() :
        width(0), height(0), pixels(0) { }


Image::Image(const char path[]) : width(0), height(0), pixels(0) {

    if (strcmp(path + strlen(path) - 3, "hdr") == 0)
        LoadRGBE(path, pixels, width, height);
    else if (strcmp(path + strlen(path) - 3, "tga") == 0)
        LoadTGA(path, pixels, width, height);
}


Image::Image(int width_in, int height_in) : width(0), height(0), pixels(0) {

    pixels = (Vector3 *) _mm_malloc(width_in * height_in * sizeof(Vector3), 16);
    width = width_in;
    height = height_in;
    clear();
}


Image::~Image() {
    _mm_free(pixels);
}


bool Image::saveHDR(const char path[]) const {
    return SaveRGBE(path, pixels, width, height);
}


bool Image::saveTonemapped(const char path[]) {
    Image &ldr = toneMap();
    if (SaveTGA(path, ldr.pixels, ldr.width, ldr.height)) {
        delete &ldr;
        return true;
    }
    else {
        delete &ldr;
        return false;
    }
}


Image &Image::toneMap() {

    const int numPixels = width * height;
    const float Yw = 33.0f;
    double Yavg = 0.0f;

    Vector3 *XYZImage = (Vector3 *) _mm_malloc(numPixels * sizeof(Vector3), 16);
    Vector3 delta = Vector3(0, 0.00391, 0);
    Matrix3x3 transform = Matrix3x3::RGBTOXYZ;

    for (int i = 0; i < numPixels; i++) {
        XYZImage[i] = (transform * pixels[i]) + delta;
    }

    Yavg = (1.0 / numPixels) * exp(Yavg);
    transform = Matrix3x3::XYZTORGB;
    Image &result = *(new Image(width, height));

    for (int i = 0; i < numPixels; i++) {

        float V = XYZImage[i][1] * (1.33f * (1.0f + ((XYZImage[i][1] + XYZImage[i][2]) / XYZImage[i][0])) - 1.68f);
        float log10Y = log10f(XYZImage[i][1]);
        float s = 1.0f;

        if (log10Y < -2.0f) s = 0.0f;
        else if (log10Y < 0.6f)
            s = (3 * pow(((log10Y + 2.0f) / 2.6f), 2.0f)) - (2 * pow(((log10Y + 2.0f) / 2.6f), 3.0f));

        float W = XYZImage[i][0] + XYZImage[i][1] + XYZImage[i][2];

        float x = XYZImage[i][0] / W;
        float y = XYZImage[i][1] / W;

        float xw = 0.34;
        float yw = 0.34; // TODO: play with these. blue lacks a bit....

        x = (1.0f - s) * xw + s * (x + xw - 0.33);
        y = (1.0f - s) * yw + s * (y + yw - 0.33);

        float Y = 0.4468 * (1.0 - s) * V + s * XYZImage[i][1];
        float tmp1 = (1.219 + powf(Yw * 0.5f, 0.4f));
        float tmp2 = (1.219 + powf(Yavg, 0.4f));
        Y = (XYZImage[i][1] / Yw) * powf((tmp1 / tmp2), 2.5f);

        float X = (x * Y) / y;
        float Z = ((1.0f - x - y) * Y) / y;

        XYZImage[i] = Vector3(X, Y, Z);
        result[i] = (transform * XYZImage[i]);
        result[i].clamp();
    }

    return result;
}


float Image::L1Norm() const {
    Vector3 brightness;
    int numPixels = width * height;

    for (int i = 0; i < numPixels; i++) {
        brightness += pixels[i];
    }
    float tmp = (brightness[0] + brightness[1] + brightness[2]) / 3.0f;
    return tmp / (float) numPixels;
}


Vector3 Image::L1NormRGB() const {

    Vector3 brightness;
    int numPixels = width * height;

    for (int i = 0; i < numPixels; i++) {
        brightness += pixels[i];
    }
    return brightness / (float) numPixels;
}


void Image::convertXYZtoRGB(Image &dest_img) {
    for (int i = 0; i < width * height; i++) {
        xyz_to_rgb(pixels[i].data, dest_img.pixels[i].data);
        dest_img.pixels[i].clampNegative();
        //pixels[i].clamp();
    }
}


void Image::clear() {
    memset(pixels, 0, width * height * sizeof(Vector3));
}