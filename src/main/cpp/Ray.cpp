#include <float.h>
#include "Vector3.h"
#include "Ray.h"

#define EPS (0.00001f)


Ray::Ray(const Vector3 &origin, const Vector3 &direction, const float tmin, const float tmax, const float lambda_in) :
        origin(origin), direction(direction), tmin(tmin), tmax(tmax), lambda(lambda_in) {

    inv_direction = Vector3(1.0f / direction[0], 1 / direction[1], 1 / direction[2]);
    sign[0] = (inv_direction[0] < 0);
    sign[1] = (inv_direction[1] < 0);
    sign[2] = (inv_direction[2] < 0);
}


Ray::Ray() :
        origin(Vector3(0, 0, 0)), direction(Vector3(0, 0, 1)), tmin(EPS), tmax(FLT_MAX), lambda(0.0f) {
}


Vector3 Ray::at(const float t) {
    return origin + t * direction;
}
