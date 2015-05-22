#include <float.h>
#include "Vector3.h"
#include "Ray.h"

#define EPS (0.00001f)


Ray::Ray(const Vector3 &origin, const Vector3 &direction, const float tMin, const float tMax) :
        origin(origin), direction(direction), tMin(tMin), tMax(tMax) {

    invDirection = Vector3(1.0f / direction[0], 1 / direction[1], 1 / direction[2]);
    sign[0] = (invDirection[0] < 0);
    sign[1] = (invDirection[1] < 0);
    sign[2] = (invDirection[2] < 0);
}


Ray::Ray() :
        origin(Vector3(0, 0, 0)), direction(Vector3(0, 0, 1)), tMin(EPS), tMax(FLT_MAX){
}


Vector3 Ray::at(const float t) {
    return origin + t * direction;
}
