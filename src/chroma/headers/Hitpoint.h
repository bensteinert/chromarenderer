#ifndef HITPOINT_H
#define HITPOINT_H

#include <cfloat>

#include "Vector3.h"
#include "Primitive.h"

class Hitpoint {

public:

    Vector3 p;                  // position
    Vector3 n;
    Vector3 t1;
    Vector3 t2;                 // 64 Byte
    bool hit;
    bool flipped;
    mutable bool inside;
    const Primitive* index;     // index of hit Primitive
    float dist;
    float u;
    float v;

    Hitpoint();

    Hitpoint(Vector3 position, Vector3 normal, Primitive *index);

    inline void getCoordSystem() {
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

    Hitpoint(const Hitpoint &hit_in);

    Hitpoint(const Vector3 &p_in, const Vector3 &normal);

    void initCoordinateSystem();

    void clear();

    const Material * getToMaterial() const;

    const Material * getFromMaterial() const;

    void clearAll();
};

#endif
