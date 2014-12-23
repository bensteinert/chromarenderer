#ifndef HITPOINT_H
#define HITPOINT_H

#include <cfloat>

class Vector3;

class Hitpoint {

public:

    Vector3 p;                // position
    Vector3 n;
    Vector3 t1;
    Vector3 t2;                // 64 Byte
    bool hit;
    bool flipped;
    mutable bool inside;
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
