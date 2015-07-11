#include <float.h>

#include "Chroma.h"
#include "Vector3.h"
#include "Ray.h"


Ray::Ray(const Vector3 &origin, const Vector3 &direction, const float tMin, const float tMax, const float lambda) :
        origin(origin), direction(direction), tMin(tMin), tMax(tMax), lambda(lambda) {

    invDirection = Vector3(1.0f / direction[0], 1 / direction[1], 1 / direction[2]);
    sign[0] = (invDirection[0] < 0);
    sign[1] = (invDirection[1] < 0);
    sign[2] = (invDirection[2] < 0);
}


Ray::Ray() :
        origin(Vector3(0, 0, 0)), direction(Vector3(0, 0, 1)), tMin(EPS), tMax(FLT_MAX), lambda(0) {
}


Vector3 Ray::at(const float t) const {
    return origin + t * direction;
}
