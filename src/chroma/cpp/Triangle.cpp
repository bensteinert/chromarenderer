#include "Chroma.h"
#include "Triangle.h"
#include "Material.h"
#include "Hitpoint.h"
#include "BoundingBox.h"
#include "Sampler.h"


Triangle::Triangle() :
        Primitive(), p0(Vector3()), p1(Vector3()), p2(Vector3()), n(Vector3()) {
}


Triangle::Triangle(float coords[9], Material *mat_in) :
        Primitive(mat_in, false, &stdAir), p0(Vector3(coords)), p1(Vector3(coords + 3)), p2(Vector3(coords + 6)),
        n(((p1 - p0) % (p2 - p0)).normalizedCopy()) {
    area = getArea();
}


Triangle::Triangle(const Vector3 &p0_in, const Vector3 &p1_in, const Vector3 &p2_in, Material *insideMat_in, Material *outsideMat_in) :
        Primitive(insideMat_in, false, outsideMat_in), p0(p0_in), p1(p1_in), p2(p2_in),
        n(((p1 - p0) % (p2 - p0)).normalizedCopy()) {
    area = getArea();
}


Triangle::~Triangle() {
}


Vector3 Triangle::getNormal(const Vector3 &pos, const Vector3 &dir) const {
    // dir is reversed ray direction in general!
    return n * ieeeSignWOif(n * dir);
}


void Triangle::getNormal(const Vector3 &dir, Hitpoint &hit) const {
    // dir is reversed ray direction in general!
    if (n * dir > 0.0f) {
        hit.n = n;
        hit.flipped = false;
    }
    else {
        hit.n = -n;
        hit.flipped = true;
    }
}


void Triangle::getBoundingBox(BoundingBox & bb) const {

    bb.pMax = maxVector(p0, maxVector(p1, p2));
    bb.pMin = minVector(p0, minVector(p1, p2));
}


float Triangle::getArea() const {

    Vector3 cross = (p1 - p0) % (p2 - p0);
    return cross.length() * 0.5f;
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


int Triangle::intersectRay(const Ray *ray, Hitpoint &hit) const {
    std::cout << "Triangle::intersectRay(const Ray *ray, Hitpoint& hit, float& u, float& v) not yet implemented!" << std::endl;
    return 0;
}


float Triangle::getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const {
    getunifdistrSample(s, position);
    normal = n;
    return area;
}


void Triangle::out(std::ostream &os) const {
    os << "P0: " << p0 << std::endl << "P1: " << p1 << std::endl << "P2: " << p2 << std::endl;
}


//void Triangle::transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation) {
//
//    p0 = rotation * (scale * p0) + translation;
//    p1 = rotation * (scale * p1) + translation;
//    p2 = rotation * (scale * p2) + translation;
//
//    n = rotation * n;
//    area = getArea();
//}



float Triangle::getunifdistrSample(Sampler &s, Vector3 &position) const {
    float u = s.genrand_real2();
    float v = s.genrand_real2();
    u = sqrt(u);
    float alpha = 1.0f - u;
    float beta = (1.0f-v) * u;
    float gamma = v * u;
    position = p0*alpha + p1*beta  + p2*gamma;
    return area;
}
