#ifndef RAY_H
#define RAY_H

#include "Vector3.h"

class Ray {

public:
    Vector3 origin, direction, invDirection;    //36
    Vector3 spectrum;                           //12
    float tMin, tMax; // interval				// 8
    float lambda;                               // 4
    char sign[3];                               // 3

    Ray();

    Ray(const Vector3 &origin, const Vector3 &direction, const float tMin, const float tMax);

    Vector3 at(const float t);

};

#endif
