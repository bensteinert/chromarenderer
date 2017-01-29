#ifndef SPHERE_H_
#define SPHERE_H_

#include "Vector3.h"
#include "Primitive.h"

class Ray;
class Material;


class Sphere : public Primitive {

public:

    Vector3 center;
    float radius;

    Sphere();

    ~Sphere();

    Sphere(float coords[4], Material *mat_in);

    Sphere(const Vector3 &c, const float rad, Material *mat_in);

    float getArea() const;

    float intersectRay(const Ray *ray, float &u, float &v) const;

    int intersectRay2(const Ray *ray, float &t1, float &t2) const;

    int intersectRay(const Ray *ray, Hitpoint &hit) const;

    float intersectSphere(const Sphere &s) const;

    float getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const;

    Vector3 getNormal(const Vector3 &pos, const Vector3 &dir) const;

    void getNormal(const Vector3 &dir, Hitpoint &hit) const;

    void getBoundingBox(BoundingBox &bb) const;

    void transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation);
};

#endif /*SPHERE_H_*/
