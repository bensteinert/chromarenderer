#include <cassert>
#include <climits>
#include <cmath>
#include <mm_malloc.h>
#include <fstream>

#include "Defines.h"
#include "BVHTree.h"


// TODO: Remove Scene from BVH ctor signature. Provide Primitive**
BVHTree::BVHTree(const Scene &scene, unsigned int maxDepth_in, unsigned int minTriangles_in, const int heuristic,
                 Primitive *lensSampler) :
        AccStruct(), root(0), maxDepth(maxDepth_in), minTris(minTriangles_in), retry(0) {

    totalNumberOfPrimitives = scene.numtris + scene.numspheres + scene.numdisks + (lensSampler != 0 ? 1 : 0);
    primitives = new Primitive *[totalNumberOfPrimitives];

    unsigned int avNodes = 2 * totalNumberOfPrimitives - 1; //TODO: crude allocation strategy....
    BVHNode *freeMem = (BVHNode *) _mm_malloc(avNodes * sizeof(BVHNode),
                                              16); // maxDepth -1 normally, but left out for rounding errors
    root = freeMem++;
    avNodes--;
    *root = BVHNode();

    indices = new unsigned int[totalNumberOfPrimitives];
    bbArray = (BoundingBox *) _mm_malloc(totalNumberOfPrimitives * sizeof(BoundingBox), 16);
    centroids = (Vector3 *) _mm_malloc(totalNumberOfPrimitives * sizeof(Vector3), 16);

    unsigned int j = 0; // global index over all Primitives

    /*build up index arrays for sorting*/
    for (int i = 0; i < scene.numtris; i++, j++) {
        primitives[j] = (Primitive *) &scene.tris[i];
        indices[j] = j;
        primitives[j]->getBoundingBox(bbArray[j]); //box will automatically infinityfied by getBoundingBox method if needed
        centroids[j] = bbArray[j].center();
    }

    for (int i = 0; i < scene.numspheres; i++, j++) {
        primitives[j] = (Primitive *) &scene.spheres[i];
        indices[j] = j;
        primitives[j]->getBoundingBox(bbArray[j]);
        centroids[j] = bbArray[j].center();
    }

//    for (int i = 0; i < scene.numdisks; i++, j++) {
//        primitives[j] = (Primitive *) &scene.disks[i];
//        indices[j] = j;
//        primitives[j]->getBoundingBox(bbArray[j]);
//        centroids[j] = bbArray[j].center();
//    }

    if (lensSampler != 0) {
        primitives[j] = lensSampler;
        indices[j] = j;
        primitives[j]->getBoundingBox(bbArray[j]);
        centroids[j] = bbArray[j].center();
        j++;
    }

    assert(j == totalNumberOfPrimitives);

    refitNode(root, 0, totalNumberOfPrimitives - 1);

    std::ofstream fs("log_bvh", std::ios::out);

    switch (heuristic) {
        case 1:
            constructTopDownBinnedSAH(root, freeMem, avNodes, 0, totalNumberOfPrimitives - 1, 1);
            fs << "\t***Binned SurfaceArea Heuristic***" << std::endl;
            fs << "\tConstruction statistics:" << std::endl;
            fs << "\tterminations due to precision:" << stat7 << std::endl;
            break;
        case 2:
            constructTopDownBestObjMed(root, freeMem, avNodes, 0, totalNumberOfPrimitives - 1, 1);
            fs << "\t***ObjectMedianSplit Heuristic***" << std::endl;
            fs << "\tConstruction statistics:" << std::endl;
            break;
        default:
            constructTopDownMidSplit(root, freeMem, avNodes, 0, totalNumberOfPrimitives - 1, 1);
            fs << "\t***SpatialMidSplit Heuristic***" << std::endl;
            fs << "\tConstruction statistics:" << std::endl;
            break;
    }

    fs << "\tallocated memory:\t\t: " << avNodes * nodeSize << " Bytes" << std::endl;
    for (int i = 0; i < MIN_TRIANGLES_PER_LEAF; i++)
        fs << "\t# leafes with " << i + 1 << " indices\t: " << stat6[i] << std::endl;
    fs << "\t# oversized leafes\t\t: " << stat1 << std::endl;
    fs << "\t# triangle bunches\t\t: " << stat2 << std::endl;
    fs << "\t# Indices of biggest leaf\t: " << stat3 << std::endl;
    fs << "\tdeepest leaf\t\t\t: " << stat5 << std::endl;
    fs << "\twasted memory\t\t\t: " << (avNodes - stat4 * 2) * nodeSize << std::endl;

    stat1 = stat2 = stat3 = stat4 = stat5 = stat7 = 0;
    _mm_free(bbArray);
    bbArray = 0;
}


BVHTree::~BVHTree() {
    _mm_free(bbArray);
    _mm_free(centroids);
    delete[] indices;
    _mm_free(root);        // Allocation of one big block by new, so []free is needed
}


void BVHTree::constructTopDownBinnedSAH(
        BVHNode *node, BVHNode *&freeMem, unsigned int freeCtr,
        const unsigned int left, const unsigned int right,
        const unsigned int actDepth, int splitaxis) {

    //bool verbose = false;
    static const unsigned int nrBins = 32;
    //static const double TraversalCost = 1000;
    //static const double IntersectionCost = 1000;
    const unsigned int numInd = right - left + 1;;

    refitNode(node, left, right);

    if (freeCtr < 2 || numInd <= minTris) {
        makeLeaf(node, left, right);
        stat5 = stat5 < actDepth ? actDepth : stat5;
        return;
    }

    const Vector3 boxExtent = node->bbox.extent();
    splitaxis = splitaxis < 0 ? boxExtent.getMaxIndex() : splitaxis;
    const double binStart = node->bbox.pMin[splitaxis];
    const double binSize = boxExtent[splitaxis] / nrBins;
    if (binSize < 0.00009) {
        makeLeaf(node, left, right);
        stat7++;
        return;
    }
    const double k1_forID = (nrBins * (1.0 - 0.0000001)) / (node->bbox.pMax[splitaxis] - node->bbox.pMin[splitaxis]);

    int binCounter[nrBins] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float costFunct[nrBins] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    BoundingBox binBB[nrBins];

    // compute counter and SA for all bins by projecting all triangleBBs in all bins
    for (unsigned int i = left; i <= right; i++) {

        assert(centroids[indices[i]][splitaxis] >= bbArray[indices[i]].pMin[splitaxis] &&
               centroids[indices[i]][splitaxis] <= bbArray[indices[i]].pMax[splitaxis]);
        unsigned int binID = (unsigned int) (k1_forID * (centroids[indices[i]][splitaxis] - binStart));
        assert(binID < nrBins && binID >= 0);
        binCounter[binID]++;
        binBB[binID].merge(bbArray[indices[i]]);
    }

    BoundingBox leftBB[nrBins];
    BoundingBox rightBB[nrBins];
    unsigned int numLeft[nrBins];
    unsigned int numRight[nrBins];

    int winner = -1; //leaf case
    float leafCost = numInd * node->bbox.surface();//costs for the leaf case
    float minCost = leafCost;
    // look for minimum cost bin config
    // go through the list of split planes defined by bins
    for (unsigned int i = 0; i < nrBins; i++) {

        numLeft[i] = 0;
        numRight[i] = 0;
        leftBB[i].infinitify();
        rightBB[i].infinitify();

        //merge all left bins
        for (unsigned int j = 0; j <= i; j++) {
            leftBB[i].merge(binBB[j]);
            numLeft[i] += binCounter[j];
        }

        //merge all right bins
        for (unsigned int j = i + 1; j < nrBins; j++) {
            rightBB[i].merge(binBB[j]);
            numRight[i] += binCounter[j];
        }

        assert((numRight[i] + numLeft[i]) == numInd);

        /*Heuristic formulation*/
        costFunct[i] = leftBB[i].surface() * numLeft[i] + rightBB[i].surface() * numRight[i];
        if (costFunct[i] < minCost) {
            winner = i;
            minCost = costFunct[i];
        }
    }

    if (winner == -1) {
        //Leaf case?
        if (++retry == 3) {
            //stop here, because bunch cant be split anymore...
            makeLeaf(node, left, right);
            stat2++;
            //cout << "\tbunch with " << node->numInd << " triangles!" << endl;
            retry = 0;
        }
        else {
            //retry with another dimension
            constructTopDownBinnedSAH(node, freeMem, freeCtr, left, right, actDepth, (splitaxis + 1) % 3);
        }
    }
    else {
        //split case
        double splitD = ((double) (winner + 1) * binSize) + binStart;
        unsigned int m = partitionTriangles(indices, centroids, splitaxis, splitD, left, right);
//		if(m-left !=numLeft[winner] || right-m+1 !=numRight[winner]){
//			cout << "left: " << m-left << " | " <<  numLeft[winner] <<  " right: " << right-m+1 << " | "<< numRight[winner]<< endl;
//		}

        if (m > right || m == left) {
            //think we lost precision somewhere...
            makeLeaf(node, left, right);
            stat7++;
            return;
        }

        node->left = freeMem++;
        freeCtr--;
        node->right = freeMem++;
        freeCtr--;
        node->left->bbox = leftBB[winner];
        node->right->bbox = rightBB[winner];
        node->axis = splitaxis;
        retry = 0;
        constructTopDownBinnedSAH(node->left, freeMem, freeCtr, left, m - 1, actDepth + 1);
        constructTopDownBinnedSAH(node->right, freeMem, freeCtr, m, right, actDepth + 1);
    }
}


void BVHTree::constructTopDownMidSplit(
        BVHNode *node, BVHNode *&freeMem, unsigned int freeCtr,
        const unsigned int left, const unsigned int right,
        const unsigned int actDepth, int splitaxis) {

    const unsigned int numInd = right - left + 1;;
    refitNode(node, left, right);

    if (freeCtr < 2 || numInd <= minTris) {
        makeLeaf(node, left, right);
        stat5 = stat5 < actDepth ? actDepth : stat5;
        return;
    }

    const Vector3 boxExtent = node->bbox.extent();
    const Vector3 boxCenter = node->bbox.center();

    splitaxis = splitaxis < 0 ? boxExtent.getMaxIndex() : splitaxis;

    float splitD = boxCenter[splitaxis];
    unsigned int m = partitionTrianglesWithPivotAdjust(indices, centroids, splitaxis, splitD, left, right);

    // crude proposal of a splitting plane
    // m is the first index pointing to the triangles on the right side
    //cout << "depth: " << actDepth << " | #l: " << m-left << " | #r: " << right-m+1 << endl;
    if (m > right || m == left || m - left < minTris / 2 || right - m + 1 < minTris / 2) {
        // case m>right is for all triangles being on the left side.
        // all triangles on the left or right side of the plane, try another axis...
        if (++retry == 3) {
            //stop here, because bunch cant be split anymore...
            makeLeaf(node, left, right);
            stat2++;
            //cout << "\tbunch with " << node->numInd << " triangles!" << endl;
            retry = 0;
            return;
        }
        else {
            constructTopDownMidSplit(node, freeMem, freeCtr, left, right, actDepth, (splitaxis + 1) % 3);
        }
    }
    else {
        retry = 0;
        node->left = freeMem++;
        freeCtr--;
        node->right = freeMem++;
        freeCtr--;
        node->axis = splitaxis;
        constructTopDownMidSplit(node->left, freeMem, freeCtr, left, m - 1, actDepth + 1);
        constructTopDownMidSplit(node->right, freeMem, freeCtr, m, right, actDepth + 1);
    }
}


void BVHTree::constructTopDownBestObjMed(BVHNode *node, BVHNode *&freeMem, unsigned int freeCtr,
                                         const unsigned int left, const unsigned int right,
                                         const unsigned int actDepth) {

    unsigned int numIndLeft[3];
    unsigned int numIndRight[3];
    int points[3];
    unsigned int m[3];
    Vector3 volume;            // summed volume of left and right subvolumes in all 3 dims
    Vector3 overlapVol;

    BoundingBox leftBB;
    BoundingBox rightBB;

    const unsigned int numInd = right - left + 1;;
    refitNode(node, left, right);

    if (freeCtr < 2 || numInd <= minTris) {
        makeLeaf(node, left, right);
        stat5 = stat5 < actDepth ? actDepth : stat5;
        return;
    }

    const Vector3 boxExtent = node->bbox.extent();
    const Vector3 boxCenter = node->bbox.center();

    float splitD;
    int splitAxis = 0;
    points[0] = points[1] = points[2] = 0;

    // split along longest axis is good... so point for that dim:
    points[boxExtent.getMaxIndex()]++;

    while (splitAxis < 3) {

        splitD = boxCenter[splitAxis];
        // m is the first index pointing to the triangles on the right side:
        m[splitAxis] = partitionTrianglesWithPivotAdjust(indices, centroids, splitAxis, splitD, left, right);

        numIndLeft[splitAxis] = m[splitAxis] - left;
        getBoundingBox(left, m[splitAxis] - 1, leftBB);
        volume[splitAxis] = leftBB.volume();

        numIndRight[splitAxis] = right - m[splitAxis] + 1;
        getBoundingBox(m[splitAxis], right, rightBB);
        volume[splitAxis] += rightBB.volume();

        overlapVol[splitAxis] = overlapVolume(&leftBB, &rightBB);
        splitAxis++;
    }

    // cull as much free space as possible
    points[volume.getMinIndex()]++;
    points[volume.getMaxIndex()]--;

    //best if overlap Volume is minimal
    points[overlapVol.getMinIndex()]++;
    points[overlapVol.getMaxIndex()]--;

    splitAxis = 0;

    while (splitAxis < 3) {

        if (numIndLeft[splitAxis] > 2 * numIndRight[splitAxis] || numIndRight[splitAxis] > 2 * numIndLeft[splitAxis]) {
            //not equally splitted triangle list sounds not good...
            points[splitAxis] -= 2;
        }
        if (numIndLeft[splitAxis] == 0 || numIndRight[splitAxis] == 0) {
            //impossible split...
            points[splitAxis] = -INT_MAX;
            //cout << "impossble" << endl;
        }
        if (numIndLeft[splitAxis] == 1 || numIndRight[splitAxis] == 1) {
            //not liked but sometimes needed....
            points[splitAxis]--;
        }
        splitAxis++;
    }

    // max points index
    int winnerDim = points[0] > points[1] ? (points[2] > points[0] ? 2 : 0) : (points[2] > points[1] ? 2 : 1);
    //cout << "Depth: " << actDepth << " | winner: " << winnerDim << " | points: " << points[0]<<points[1]<<points[2] << endl;
    if (winnerDim < 0) {
        //triangle bunch... splitting is futile!
        makeLeaf(node, left, right);
        //cout << "triangle bunch with " << numInd << " triangles..." << endl;
        stat5 = stat5 < actDepth ? actDepth : stat5;
        return;
    }
    else {
        int split = partitionTrianglesWithPivotAdjust(indices, centroids, winnerDim, boxCenter[winnerDim], left, right);
        node->left = freeMem++;
        freeCtr--;
        node->right = freeMem++;
        freeCtr--;
        node->axis = winnerDim;
        constructTopDownBestObjMed(node->left, freeMem, freeCtr, left, split - 1, actDepth + 1);
        constructTopDownBestObjMed(node->right, freeMem, freeCtr, split, right, actDepth + 1);
    }
}


inline void BVHTree::getBoundingBox(const unsigned int left, const unsigned int right, BoundingBox &bbox) {

    bbox = BoundingBox(); //std constructor creates infinitified box

    for (unsigned int i = left; i <= right; i++) {
        bbox.merge(bbArray[indices[i]]);
    }
}


inline void BVHTree::refitNode(BVHNode *const node, const unsigned int left, const unsigned int right) {
    getBoundingBox(left, right, node->bbox);
}


inline void BVHTree::makeLeaf(BVHNode *const node, const unsigned int left, const unsigned int right) {
    node->left = node->right = 0;
    node->numInd = right - left + 1;
    node->indexList = &(indices[left]);

    stat3 = node->numInd > stat3 ? node->numInd : stat3;
    if (node->numInd > minTris)
        ++stat1;
    else {
        stat6[node->numInd - 1]++;
    }
    ++stat4;
}


void BVHTree::intersect(Ray &ray, Hitpoint &hit) const {
    DBG_ASSERT(!hit.hit);
    hit.dist = ray.tMax;

    if (root->bbox.rayBoxIntersection(ray)) {
        intersectRec(ray, *root, hit, hit.index);
    }
}


void BVHTree::intersectRec(Ray &ray, const BVHNode &node, Hitpoint &hit, const Primitive *const rejIndex) const {

    if (!node.left) {
        //Leaf case
        Primitive *actual = 0;
        float dist;
        float u = 0.0f, v = 0.0f;

        for (unsigned int t = 0; t < node.numInd; t++) {

            actual = primitives[node.indexList[t]];

            if (!actual->closed && rejIndex == actual) {
                continue;
            }

            dist = actual->intersectRay(&ray, u, v);
            ++stat1;

            //hit...?
            if (dist > ray.tMin && dist < hit.dist) {
                hit.hit = true;
                hit.index = actual;
                hit.p = ray.at(dist);
                actual->getNormal(-ray.direction, hit);
                //DBG_ASSERT(equals(hit.n.length(),1.0f));
                hit.dist = dist;
                hit.u = u;
                hit.v = v;
            }
        }
        return;
    }
    else {

        float tmin_left = 0.0f, tmax_left = 0.0f;
        float tmin_right = 0.0f, tmax_right = 0.0f;

        BVHNode *left = node.left;
        BVHNode *right = node.right;

        unsigned char hitBits = 0;
        hitBits = hitBits | (unsigned char) left->bbox.rayBoxIntersection(ray, tmin_left, tmax_left);
        hitBits = hitBits | (unsigned char) right->bbox.rayBoxIntersection(ray, tmin_right, tmax_right) << 1;

        switch (hitBits) {
            case 1: // left hit
                intersectRec(ray, *left, hit, rejIndex);
                break;
            case 2: // right hit
                intersectRec(ray, *right, hit, rejIndex);
                break;
            case 3: // both hit

                if (tmin_left <= tmin_right) {
                    intersectRec(ray, *left, hit, rejIndex);
                    if (hit.dist > tmin_right)  // incl
                        intersectRec(ray, *right, hit, rejIndex);
                }
                else {
                    intersectRec(ray, *right, hit, rejIndex);
                    if (hit.dist > tmin_left)
                        intersectRec(ray, *left, hit, rejIndex);
                }
                break;
        }
    }
}


bool BVHTree::visible(Ray &ray, const Primitive *const rejIndex, const Primitive *const destIndex) const {

    Hitpoint hit;
    hit.index = destIndex;                                //destIndex is received by sampling, so hit can be assumed...
    visibleRec(ray, *root, hit, rejIndex, destIndex);
    return hit.index == destIndex;
}


void BVHTree::visibleRec(Ray &ray, const BVHNode &node, Hitpoint &hit, const Primitive *const rejIndex,
                         const Primitive *const destIndex) const {

    if (node.bbox.rayBoxIntersection(ray)) {

        if (!node.left) {
            //Leaf case
            float dist;
            Primitive *actual;
            float u = 0.0f, v = 0.0f;

            for (unsigned int t = 0; t < node.numInd; t++) {

                actual = primitives[node.indexList[t]];

                if (!actual->closed && rejIndex == actual) continue; // self intersection test....

                dist = actual->intersectRay(&ray, u, v);
                ++stat1;

                if (dist > ray.tMin && dist < ray.tMax) {
                    hit.hit = true;
                    hit.index = actual;
                    if (actual != destIndex)
                        return;                        // obj object in the path
                    else
                        continue;                    // can happen due to floating point precision loss
                }
            }
        }
        else {
            //Node
            visibleRec(ray, *node.left, hit, rejIndex, destIndex);

            if (hit.index == destIndex) {
                visibleRec(ray, *node.right, hit, rejIndex, destIndex);
            }
        }
    }
}
