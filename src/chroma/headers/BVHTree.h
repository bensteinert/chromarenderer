#ifndef BVHTREE_H_
#define BVHTREE_H_

#include "Primitive.h"
#include "BoundingBox.h"
#include "Hitpoint.h"
#include "Ray.h"
#include "Scene.h"
#include "AccStruct.h"
#include "BVHNode.h"


class BVHTree : public AccStruct {
public:
    BVHNode *root;
    unsigned int *indices;
    const static unsigned char nodeSize = sizeof(BVHNode);

    BVHTree(const Scene &scene, unsigned int maxDepth, unsigned int minTriangles, int heutristic = 0,
            Primitive *lensSampler = 0);

    ~BVHTree();

    void intersect(Ray &ray, Hitpoint &hitpoint) const;

    bool visible(Ray &ray, const Primitive *const rejIndex, const Primitive *const destIndex = 0) const;


private:
    unsigned int maxDepth;
    unsigned int minTris;
    BoundingBox *bbArray;
    Vector3 *centroids;
    unsigned char retry;

    /**
     * during construction time:
     * stat1: number of oversized leafes
     * stat2: number of not seperable triangle bunches
     * stat3: biggest Leaf
     **/

    void constructTopDownMidSplit(BVHNode *node, BVHNode *&freeMem, unsigned int freeCtr, const unsigned int left,
                                  const unsigned int right, const unsigned int actDepth, int splitaxis = -1);

    void constructTopDownBestObjMed(BVHNode *node, BVHNode *&freeMem, unsigned int freeCtr, const unsigned int left,
                                    const unsigned int right, const unsigned int actDepth);

    void constructTopDownBinnedSAH(BVHNode *node, BVHNode *&freeMem, unsigned int freeCtr, const unsigned int left,
                                   const unsigned int right, const unsigned int actDepth, int splitaxis = -1);

    inline void refitNode(BVHNode *const node, const unsigned int left, const unsigned int right);

    inline void getBoundingBox(const unsigned int left, const unsigned int right, BoundingBox &bbox);

    inline void makeLeaf(BVHNode *const node, const unsigned int left, const unsigned int right);

    void intersectRec(Ray &ray, const BVHNode &node, Hitpoint &hit, const Primitive *const rejIndex) const;

    //void visibleRec  (Ray& ray, const BVHNode& node, Hitpoint &hit, const Primitive* const rejIndex) const;
    void visibleRec(Ray &ray, const BVHNode &node, Hitpoint &hit, const Primitive *const rejIndex,
                    const Primitive *const destIndex) const;

};

#endif
