#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Vector3.h"
#include "Primitive.h"

class Material;

class Triangle : public Primitive {
public:
    Vector3 p0, p1, p2;
    Vector3 n;

    Triangle();
    ~Triangle();
    Triangle(float coords[9], Material *mat_in);
    Triangle(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, Material *insideMat_in, Material *outsideMat_in);

    void getBoundingBox(BoundingBox &bb) const;

    float intersectRay(const Ray *ray, float &u, float &v) const;

    int intersectRay(const Ray *ray, Hitpoint &hit) const;

    float getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const;

    Vector3 getNormal(const Vector3 &pos, const Vector3 &dir) const;

    void getNormal(const Vector3 &dir, Hitpoint &hit) const;

    float getArea() const;

    //void transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation);

    void out(std::ostream &os) const;

    float getunifdistrSample(Sampler &s, Vector3 &position) const;

};

#endif /*TRIANGLE_H_*/
