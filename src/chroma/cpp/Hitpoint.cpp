#include "Vector3.h"
#include "Hitpoint.h"


Hitpoint::Hitpoint() :
        p(Vector3()), n(Vector3()), t1(Vector3()), t2(Vector3()),
        hit(false), flipped(false), inside(false), index(0), dist(FLT_MAX), u(0.0f), v(0.0f) {
}


Hitpoint::Hitpoint(const Hitpoint &hit_in) :
        p(hit_in.p), n(hit_in.n), t1(hit_in.t1), t2(hit_in.t2),
        hit(false), flipped(hit_in.flipped), index(hit_in.index), inside(false), dist(FLT_MAX), u(hit_in.u),
        v(hit_in.v) {
}


Hitpoint::Hitpoint(const Vector3 &p_in, const Vector3 &normal) :
        p(p_in), n(normal), t1(Vector3()), t2(Vector3()),
        hit(false), flipped(false), inside(false), dist(FLT_MAX), u(0.0f), v(0.0f) {
    initCoordinateSystem();
}


Hitpoint::Hitpoint(Vector3 position, Vector3 normal, Primitive *index) :
        p(position), n(normal), t1(Vector3()), t2(Vector3()), hit(false), flipped(false), inside(false),
        dist(FLT_MAX), u(0.0f), v(0.0f), index(index) {
}


void Hitpoint::initCoordinateSystem() {

#ifdef _DEBUG
		if(n.length()<=0.0f) std::cerr << "undefined normal found during coord Setup for hitpoint!" << std::endl;
    #endif

    t1 = n;
    t1[t1.getMinIndexAbs()] = 1.0f;
    t1 = t1 % n;
    t1.normalize();
    t2 = n % t1;
    t2.normalize();
}


void Hitpoint::clear() {
    dist = FLT_MAX;
    hit = false;
    u = 0.0f;
    v = 0.0f;
    p = Vector3();
    n = Vector3();
    t1 = Vector3();
    t2 = Vector3();
}


const Material inline *Hitpoint::getToMaterial() const {
    return inside ? index->outsideMat : index->insideMat;
}


const Material inline *Hitpoint::getFromMaterial() const {
    return inside ? index->insideMat : index->outsideMat;
}


void Hitpoint::clearAll() {
    clear();
    flipped = false;
    index = 0;
}
