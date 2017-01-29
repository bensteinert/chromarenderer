#include "BVHNode.h"


BVHNode::BVHNode() : left(0), right(0), indexList(0), axis(-1) {
    bbox.infinitify();
}


BVHNode::BVHNode(const Vector3 &pMin_in, const Vector3 &pMax_in) :
        bbox(BoundingBox(pMin_in, pMax_in)), left(0), right(0), indexList(0), axis(-1) { }


BVHNode::~BVHNode() {
}
