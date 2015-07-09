#include "Defines.h"
#include "BoundingBox.h"
#include "Disk.h"
#include "Chroma.h"
#include "Hitpoint.h"


Disk::Disk() { }


Disk::Disk(const Vector3 &center_in, const Vector3 &normal_in, const float &radius_in, Material *insideMat_in,
           Material *outsideMat_in) :
        Primitive(insideMat_in, false, outsideMat_in), center(center_in), normal(normal_in), radius(radius_in),
        radiusSquared(radius_in * radius_in) {
    area = getArea();
}


Disk::Disk(const Disk &in) :
        Primitive(in.insideMat, false, in.outsideMat), center(in.center), normal(in.normal),
        radius(in.radius), radiusSquared(in.radiusSquared) {
    area = getArea();
}


Disk::~Disk() { }


void Disk::getBoundingBox(BoundingBox &bb) const {
    // Monte Carlo approx....
    static const int numSamples = 512;
    Vector3 *samples = 0;
    bb.infinitify();

    //WARNING! usage of globalsampler is not multithread-safe!
    // TODO: Needed for thin lens camera!!!
    //getunifdistrSamples(*globalSampler, numSamples, samples);

    for (int i = 0; i < numSamples; i++)
        bb.insertVertex(samples[i]);

    delete[] samples;
}


float Disk::getArea() const {
    return PI_f * radius * radius;
}


float planeRayIntersection(const Ray& ray, const Vector3& normal, const Vector3& center){
    //general ray equation placed into plane equation:
    //0 = (H-P) * n
    //H = O + t*d
    //0 = (O+t*d-P)*n

    float OminusPmultN = (ray.origin - center)*normal;
    float DmultN = ray.direction*normal;

    if(DmultN!=0.0f){
        //ray and plane are not parallel
        float t = OminusPmultN/DmultN;
        return -t;
    }

    return 0.0f;
}


float Disk::intersectRay(const Ray *ray, float &u, float &v) const {

    const float t = planeRayIntersection(*ray, normal, center);

    if (t > 0.0f) {
        Vector3 tmpHit = ray->at(t);
        float distSq = (tmpHit - center).lengthSquared();
        if (distSq < radiusSquared)
            return t;
    }
    return 0.0f;
}


int Disk::intersectRay(const Ray *ray, Hitpoint &hit) const {

    const float t = planeRayIntersection(*ray, normal, center);

    if (t > 0.0f) {
        Vector3 tmpHit = ray->at(t);
        float distSq = (tmpHit - center).lengthSquared();
        if (distSq < radiusSquared) {
            hit.hit = true;
            hit.index = this;
            hit.dist = t;
            hit.u = 0.0f;
            hit.v = 0.0f;
            hit.p = tmpHit;
            getNormal(-ray->direction, hit);
            return 1;
        }
        else {
            return 0;
        }
    }
    return 0;
}


float  Disk::getunifdistrSamples(Sampler &s, const int nrSamples, Vector3 *&samples) const {
    //write samples to global SamplerArray
    Vector3 t1, t2;
    getCoordSystem(normal, t1, t2);
    samples = new Vector3[nrSamples];

    for (int i = 0; i < nrSamples; i++) {
        Vector3 sample = (s.get_unifdistr_DiskSample() * radius);
        samples[i] = (sample[0] * t1 + sample[1] * t2 + sample[2] * normal) + center;
    }
    return area;
}


float Disk::getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &n) const {
    n = normal;
    return getunifdistrSample(s, position);
}


Vector3 Disk::getNormal(const Vector3 &pos, const Vector3 &dir) const {
    return ieeeSignWOif(normal * dir) * normal;
}


void Disk::getNormal(const Vector3 &dir, Hitpoint &hit) const {
    // dir is reversed ray direction in general!
    if (normal * dir > 0.0f) {
        hit.n = normal;
        hit.flipped = false;
    }
    else {
        hit.n = -normal;
        hit.flipped = true;
    }
}


void Disk::transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation) {

    center = (rotation * (scale * center)) + translation;
    normal = rotation * normal;
    radius = radius * scale;
    radiusSquared = radius * radius;
    area = getArea();
}


void Disk::out(std::ostream &os) const {
    os << "c=" << center << " | " << "n=" << normal << " | " << "r=" << radius << std::endl;
}


void Disk::shift(const Vector3 &v) {
    center += v;
}


void Disk::scaleRel(float scale) {
    center *= scale;
    radius *= scale;
    radiusSquared = radius * radius;
    area = getArea();
}


void Disk::scaleAbs(float val) {
    float scale = val / radius;
    scaleRel(scale);
}


void Disk::stopUp() {
    scaleToRadius(radius / SQRT2);
}


void Disk::stopDown() {
    scaleToRadius(radius * SQRT2);
}


float Disk::getunifdistrSample(Sampler &s, Vector3 &position) const {
    Vector3 sample = (s.get_unifdistr_DiskSample() * radius);
//		assert(sample.length()<radius);
    Vector3 t1, t2;
    getCoordSystem(normal, t1, t2);
    position = (sample[0] * t1 + sample[1] * t2 + sample[2] * normal) + center;
    return area;
}


void Disk::scaleToRadius(const float rad_in) {
    radius = rad_in;
    radiusSquared = rad_in * rad_in;
    area = getArea();
}