#ifndef CHROMASTRUCTS_H
#define CHROMASTRUCTS_H

#include "Vector3.h"
#include "Defines.h"

class Primitive;

typedef struct{
    float radius;;
    Vector3 center;
    float area;
} Pupil;

typedef struct{
    Vector3 position;
    Vector3 power;
    const Primitive* index;
    int padding[3]; //TODO: ugly
} _MM_ALIGN16 VPL;

typedef struct{
    Vector3 pos;
    Primitive* t;
    float q;
    float wl;
    int padding;
} _MM_ALIGN16 photon;


#endif /*CHROMASTRUCTS_H*/
