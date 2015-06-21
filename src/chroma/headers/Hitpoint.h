#ifndef HITPOINT_H
#define HITPOINT_H

#include <cfloat>

#include "Vector3.h"
#include "Primitive.h"

class Hitpoint {

public:

    Hitpoint(Vector3 position, Vector3 normal, Primitive *index);

    Vector3 p;                // position
    Vector3 n;
    Vector3 t1;
    Vector3 t2;                // 64 Byte
    bool hit;
    bool flipped;
    mutable bool inside;
    const Primitive* index; // index of hit Primitive
    float dist;
    float u;
    float v;

    Hitpoint();

    Hitpoint(const Hitpoint &hit_in);

    Hitpoint(const Vector3 &p_in, const Vector3 &normal);

    void initCoordinateSystem();

    void clear();

};

#endif
