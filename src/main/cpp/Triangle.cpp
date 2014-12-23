#include "Triangle.h"
#include "Ray.h"
#include "Hitpoint.h"


Triangle::Triangle() :
        p0(Vector3()), p1(Vector3()), p2(Vector3()),
        n(Vector3()) {
}


Triangle::Triangle(float coords[9]) :
        p0(Vector3(coords)), p1(Vector3(coords + 3)), p2(Vector3(coords + 6)),
        n(((p1 - p0) % (p2 - p0)).normalizedCopy()) {
}


Triangle::Triangle(const Vector3 &p0_in, const Vector3 &p1_in, const Vector3 &p2_in) :
        p0(p0_in), p1(p1_in), p2(p2_in),
        n(((p1 - p0) % (p2 - p0)).normalizedCopy()) {
}


Triangle::~Triangle() {
}


float Triangle::intersectRay(const Ray *ray, float &u, float &v) const {
    Vector3 E1, E2, P, Q, T;
    float det;

    //precompute?
    E1 = p1 - p0;
    E2 = p2 - p0;

    /* begin calculating determinant - also used to calculate U parameter */
    P = ray->direction % E2;

    /* if determinant is near zero, ray lies in plane of triangle */
    det = E1 * P;

#ifdef BACKFACE_CULLING

    /* calculate distance from vert0 to ray origin */
    T = ray->origin - p0;

    /* calculate U parameter and test bounds */
    u = T * P;
    if(u < 0.0f || u > det)
        return 0;

    /* prepare to test V parameter */
    Q = T % E1;

    /* calculate V parameter and test bounds */
    v = ray->direction * Q;
    if(v < 0.0f || u + v > det)
        return 0;

    det = 1./det;
    u *= det;
    v *= det;
    /* calculate t, scale parameters, ray intersects triangle */
    return E2 * Q * det;
#else
    det = 1.0 / det;

    /* calculate distance from vert0 to ray origin */
    T = ray->origin - p0;

    /* calculate U parameter and test bounds */
    u = T * P * det;
    if (u < 0.0f || u > 1.0f)
        return 0;

    /* prepare to test V parameter */
    Q = T % E1;

    /* calculate V parameter and test bounds */
    v = ray->direction * Q * det;
    if (v < 0.0f || u + v > 1.0f)
        return 0;

    /* calculate t, ray intersects triangle */
    return E2 * Q * det;

    // return t without respect to ray interval! That will be done by traversal code!

#endif
}


