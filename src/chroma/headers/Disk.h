#ifndef DISK_H_
#define DISK_H_

#include "Primitive.h"
#include "Matrix3x3.h"
#include "Sampler.h"

class Disk : public Primitive {

public:

    Vector3 center;        //Point where cone has "radius" 0; //peak
    Vector3 normal;
    float radius;
    float radiusSquared;        // cheaper for Intersection Test

    Disk();

    Disk(const Disk &in);

    Disk(const Vector3 &center_in, const Vector3 &normal_in, const float &radius_in, Material *insideMat, Material *outsideMat);

    ~Disk();

    void getBoundingBox(BoundingBox &bb) const;

    float getArea() const;

    float intersectRay(const Ray *ray, float &u, float &v) const;

    int intersectRay(const Ray *ray, Hitpoint &hit) const;

    float getunifdistrSamples(Sampler &s, const int nrSamples, Vector3 *&samples) const;

    float getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const;

    float getunifdistrSample(Sampler &s, Vector3 &position) const;

    Vector3 getNormal(const Vector3 &pos, const Vector3 &dir) const;

    void getNormal(const Vector3 &dir, Hitpoint &hit) const;

    void transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation);

    void out(std::ostream &os) const;

    void shift(const Vector3 &v);

    void scaleRel(float scale);

    void scaleAbs(float val);

    void stopUp();

    void stopDown();


    void scaleToRadius(const float rad_in);


private:

};

#endif /*DISK_H_*/
