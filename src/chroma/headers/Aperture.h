#ifndef APERTURE_H_
#define APERTURE_H_

#include "Structs.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "BoundingBox.h"
#include "Triangle.h"
#include "Disk.h"


class Aperture : public Primitive {

public:
    bool circular;
    float radius;
    int numTris;
    Triangle *tris;
    Vector3 center;
    Vector3 normal;

    Aperture();

    Aperture(const Aperture &ap_in);

    Aperture(int numBlades, const Vector3 &center, float radius_in);

    Aperture(const int numVerts, const Vector3 *vertices, float radius_in);

    ~Aperture();

    void getBoundingBox(BoundingBox &bb) const;

    float getArea() const;

    float intersectRay(const Ray *ray, float &u, float &v) const;


    void init(int numBlades, const Vector3 &center_in, float radius_in);

    int intersectRay(const Ray *ray, Hitpoint &hit) const;

    float getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const;

    float getunifdistrSample(Sampler &s, const Pupil &pupilData, Vector3 &position, Vector3 &normal) const;

    float getunifdistrSample(Sampler &s, Vector3 &position) const;

    float getunifdistrSamples(Sampler &s, const int nrSamples, Vector3 *&samples) const;

    Vector3 getNormal(const Vector3 &pos, const Vector3 &dir) const;

    void getNormal(const Vector3 &dir, Hitpoint &hit) const;

    void transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation);

    void out(std::ostream &os) const;

    void shift(const Vector3 &v);

    void scaleRel(float scale);

    void scaleAbs(float val);

    void stopUp();

    void stopDown();


    inline void scaleToRadius(float rad) {
        if (circular) circAP.scaleToRadius(rad);
        else {
            float scale = rad / radius;    ///radius for normalizing * scale for transformation
            for (int i = 0; i < numTris; i++) {
                tris[i].p0 = (tris[i].p0 - center) * scale + center;
                tris[i].p1 = (tris[i].p1 - center) * scale + center;
                tris[i].p2 = (tris[i].p2 - center) * scale + center;
            }
        }

        radius = rad;
        area = getArea();
    }


    inline void integrityCheck() const {
        assert(objectID == APID);
    }


private:
    int objectID;
    mutable Disk circAP;
};

#endif /*APERTURE_H_*/
