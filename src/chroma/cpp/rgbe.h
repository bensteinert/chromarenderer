#ifndef _H_RGBE
#define _H_RGBE

#include <stdio.h>
#include "Vector3.h"

typedef struct {
	int valid;           
	char programtype[16];
	float gamma;         
	float exposure;      
} rgbe_header_info;


#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA       0x02
#define RGBE_VALID_EXPOSURE    0x04

#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

int RGBE_WriteHeader(FILE *fp, int width, int height, rgbe_header_info *info);
int RGBE_ReadHeader(FILE *fp, int *width, int *height, rgbe_header_info *info);


int RGBE_WritePixels(FILE *fp, float *data, int numpixels);
int RGBE_ReadPixels(FILE *fp, float *data, int numpixels);

int RGBE_WritePixels_RLE(FILE *fp, float *data, int scanline_width, int num_scanlines);
int RGBE_ReadPixels_RLE(FILE *fp, float *data, int scanline_width, int num_scanlines);

bool LoadRGBE(const char *filename,Vector3*& pixels, int& width, int& height);
bool SaveRGBE(const char *filename, const Vector3* const &pixels, const int& width, const int& height);

#endif
