#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

class Ray;
class Material;
class Hitpoint;
class Sampler;
class BoundingBox;
class Vector3;
class Matrix3x3;


class Primitive {

public:
    Material *insideMat;        // 4
    Material *outsideMat;        // 4
    bool closed;
    bool plane;
    float area;                    //m²!!

    Primitive();

    Primitive(Material *inMat_in, bool closedPrimitive, Material *outMat_in);

    virtual ~Primitive();

    virtual void getBoundingBox(BoundingBox &bb) const = 0;

    virtual float getArea() const = 0;

    virtual float intersectRay(const Ray *ray, float &u, float &v) const = 0;

    virtual int intersectRay(const Ray *ray, Hitpoint &hit) const = 0;

    virtual float getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const = 0;

    //virtual float getunifdistrSample(Sampler &s, const pupil &p, Vector3 &position, Vector3 &normal) const;

    virtual Vector3 getNormal(const Vector3 &pos, const Vector3 &dir) const = 0;

    virtual void getNormal(const Vector3 &dir, Hitpoint &hit) const = 0;

    //virtual void transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation);

};

#endif /*PRIMITIVE_H_*/
