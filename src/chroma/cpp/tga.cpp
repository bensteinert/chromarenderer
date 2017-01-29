#include <stdio.h>
#include <cstring>

#include "Vector3.h"


bool LoadTGA(const char filename[], Vector3*& image, int &width, int &height) {

    FILE *f = fopen(filename, "rb");

    if (!f) return false;

    // header
    unsigned char header[18];
    if (fread(header, 1, sizeof(header), f) != sizeof(header)) return false;

    // image info
    width = ((unsigned int) header[13] << 8) | header[12]; // width as short
    height = ((unsigned int) header[15] << 8) | header[14]; // height as short
    if (header[2] != 2) return false;

    // allocate image memory
    const unsigned int rowSize = width * 3;
    unsigned char * linebuffer = new unsigned char[rowSize];
    image = new Vector3[height*width];

    // image data
    for (int y = height-1; y >= 0; y--) { // flip vertically

        if (fread(linebuffer, 1, rowSize, f) != rowSize) {
            delete [] image;
            delete [] linebuffer;
            image = 0;
            return false;
        }
        // flip BGR => RGB
        for (int x = 0; x < width; x++) {
            image[y*width + x] = Vector3((float)linebuffer[x*3+2]/255.0f,(float)linebuffer[x*3+1]/255.0f,(float)linebuffer[x*3+0]/255.0f);
            //	std::cout << 	image[y*width + x] << std::endl;
        }
    }

    fclose(f);
    delete [] linebuffer;

    return true;
}

bool SaveTGA(const char filename[], Vector3* toSave, const int& width, const int& height) {

    FILE *f = fopen(filename, "wb");
    if (!f)return false;

    // header
    unsigned char header[18];
    memset(header, 0, sizeof(header));
    header[2] = 2; // grayscale or RGB without RLE
    header[12] = (unsigned char) width; // width as short
    header[13] = (unsigned char) (width >> 8);
    header[14] = (unsigned char) height; // height as short
    header[15] = (unsigned char) (height >> 8);
    header[16] = 24; // BPP
    if (fwrite(header, 1, sizeof(header), f) != sizeof(header))
        return false;

    // image data
    unsigned int rowSize = width*3;
    unsigned char *row = new unsigned char[rowSize]; // cache one line

    for (int y = height-1; y >= 0; y--) { // flip vertically
        for (int x = 0; x < width; x++) { // swap RGB => BGR

            Vector3 ref = toSave[y*width + x]*255.0f;
            unsigned char * const dst = row + x * 3;
            dst[0] = (unsigned char)(ref[2]);
            dst[1] = (unsigned char)(ref[1]);
            dst[2] = (unsigned char)(ref[0]);
        }

        if (fwrite(row, 1, rowSize, f) != rowSize) {
            delete [] row;
            return false;
        }
    }
    delete [] row;

    fclose(f);

    return true;
}

