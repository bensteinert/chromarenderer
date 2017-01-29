#ifndef CHROMASTRUCTS_H
#define CHROMASTRUCTS_H

#include "Vector3.h"
#include "Defines.h"
#include "Sampler.h"

class Primitive;

typedef struct{
    float radius;;
    Vector3 center;
    float area;
} Pupil;

typedef struct{
    int rayCount;
    int visCount;
    int avgDepth;
    int avglensDepth;
    int lensRayCount;
    int diffracted;
    int lensBodyHitCount;
    int innerReflectionCount;
    int positives;
    int negatives;
    float debugLambda;
    unsigned int LensTraceTime;
    unsigned int RenderTime;
    unsigned int TotalTime;
    unsigned int TraversalTime;
    unsigned int ShadingTime;
    unsigned char debugColorFlag;
    Sampler sampler;

} ThreadEnv;

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
