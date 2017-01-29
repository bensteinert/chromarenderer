#ifndef ACCSTRUCT_H_
#define ACCSTRUCT_H_

#include <cstring>

class Primitive;
class Ray;
class Hitpoint;


class AccStruct {

public:

    Primitive **primitives;
    unsigned int totalNumberOfPrimitives;

    mutable unsigned int stat1, stat2, stat3, stat4, stat5, stat7;
    mutable unsigned int stat6[MIN_TRIANGLES_PER_LEAF];


    AccStruct() : primitives(0), totalNumberOfPrimitives(0), stat1(0), stat2(0), stat3(0), stat4(0), stat5(0), stat7(0) {
        std::memset(stat6, 0, MIN_TRIANGLES_PER_LEAF * 4);
    };

    virtual ~AccStruct() { delete[] primitives; };

    virtual void intersect(Ray &ray, Hitpoint &hitpoint) const = 0;

    virtual bool visible(Ray &ray, const Primitive *rejIndex, const Primitive *destIndex) const = 0;



};

#endif /*ACCSTRUCT_H_*/
