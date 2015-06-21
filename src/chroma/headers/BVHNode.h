#ifndef BVHNODE_H_
#define BVHNODE_H_

#include "Vector3.h"
#include "BoundingBox.h"

class BVHNode {
public:
    BoundingBox bbox;         // 32
    BVHNode *left;            // 4
    BVHNode *right;           // 4
    unsigned int numInd;      // 4
    unsigned int *indexList;  // 4
    unsigned char axis;       // 1

    BVHNode();

    BVHNode(const Vector3 &pMin_in, const Vector3 &pMax_in);

    ~BVHNode();
} __attribute__ ((aligned (16)));

#endif
