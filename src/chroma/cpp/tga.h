#ifndef TGA_H
#define TGA_H

class Vector3;

bool SaveTGA(const char filename[], Vector3 *toSave, const int &width, const int &height);

bool LoadTGA(const char filename[], Vector3 *&image, int &width, int &height);

#endif