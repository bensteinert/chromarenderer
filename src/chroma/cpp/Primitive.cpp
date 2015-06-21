#include "Chroma.h"
#include "Primitive.h"


Primitive::Primitive() :
        insideMat(&stdNull), outsideMat(&stdNull), closed(false), plane(!closed), area(0.0f) { }


Primitive::Primitive(Material *inMat_in, bool closedPrimitive, Material *outMat_in) :
        insideMat(inMat_in), outsideMat(outMat_in), closed(closedPrimitive), plane(!closed), area(0.0f) { }


Primitive::~Primitive() {
}

