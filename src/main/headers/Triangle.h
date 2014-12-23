#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Vector3.h"

class Ray;
class Hitpoint;

class Triangle {
public:
    Vector3 p0, p1, p2;
    Vector3 n;

    Triangle();

    ~Triangle();

    Triangle(float coords[9]);

    Triangle(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2);

    float intersectRay(const Ray *ray, float &u, float &v) const;

};

#endif /*TRIANGLE_H_*/
