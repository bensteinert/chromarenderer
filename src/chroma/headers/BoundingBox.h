#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#pragma GCC diagnostic ignored "-Wchar-subscripts"

#include <mm_malloc.h>
#include <cfloat>
#include <cassert>


class BoundingBox {

public:
    Vector3 pMin, pMax;


    BoundingBox() : pMin(Vector3(FLT_MAX, FLT_MAX, FLT_MAX)), pMax(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) { };


    BoundingBox(const Vector3 &min, const Vector3 &max) : pMin(min), pMax(max) { };


    ~BoundingBox() { };


    inline Vector3 center() const {

        return pMin + ((pMax - pMin) * 0.5f);
    }


    inline float center(const int &dim) const {

        return pMin[dim] + ((pMax[dim] - pMin[dim]) * 0.5f);
    }


    inline float surface() const {

        Vector3 ext = extent();
        if (ext < 0.0f)
            return 0.0f;
        else {
            float x_y = ext[0] * ext[1] * 2.0f;
            float y_z = ext[1] * ext[2] * 2.0f;
            float x_z = ext[0] * ext[2] * 2.0f;
            return x_y + y_z + x_z;
        }
    }


    inline float volume() const {

        Vector3 extent = pMax - pMin;
        return extent[0] * extent[1] * extent[2];
    }


    inline Vector3 extent() const {

        return pMax - pMin;
    }


    inline void infinitify() {

        pMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        pMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    }


    inline void uninfinitify() {

        static BoundingBox zerobox = BoundingBox(Vector3(0, 0, 0), Vector3(0, 0, 0));
        merge(zerobox);
    }


    friend inline float overlapVolume(const BoundingBox *bb1, const BoundingBox *bb2) {

        if (bb2->pMin << bb1->pMin) { // less test for all dimensions "anded"
            //bb2 is nearer to origin. Switch pointer
            const BoundingBox *tmp = bb1;
            bb1 = bb2;
            bb2 = tmp;
        }

        if (bb1->pMax >> bb2->pMin) {// greater test for all dimensions "anded"
            // overlap...
            Vector3 extent = bb1->pMax - bb2->pMin;
            return extent[0] * extent[1] * extent[2];
        }
        else return 0;
    }


    inline void merge(const BoundingBox &bb) {

        //TODO:SSE
        pMin[0] = bb.pMin[0] < pMin[0] ? bb.pMin[0] : pMin[0];
        pMin[1] = bb.pMin[1] < pMin[1] ? bb.pMin[1] : pMin[1];
        pMin[2] = bb.pMin[2] < pMin[2] ? bb.pMin[2] : pMin[2];

        pMax[0] = bb.pMax[0] > pMax[0] ? bb.pMax[0] : pMax[0];
        pMax[1] = bb.pMax[1] > pMax[1] ? bb.pMax[1] : pMax[1];
        pMax[2] = bb.pMax[2] > pMax[2] ? bb.pMax[2] : pMax[2];

    }


    inline void insertVertex(const Vector3 &p) {

        pMin = minVector(pMin, p);
        pMax = maxVector(pMax, p);
    }


    inline bool rayBoxIntersection(const Ray &ray, float &box_tmin, float &box_tmax) const {

        float xmin, xmax, ymin, ymax, zmin, zmax;
        Vector3 bounds[2] = {pMin, pMax};

        xmin = (bounds[ray.sign[0]][0] - ray.origin[0]) * ray.invDirection[0];
        xmax = (bounds[1 - ray.sign[0]][0] - ray.origin[0]) * ray.invDirection[0];
        ymin = (bounds[ray.sign[1]][1] - ray.origin[1]) * ray.invDirection[1];
        ymax = (bounds[1 - ray.sign[1]][1] - ray.origin[1]) * ray.invDirection[1];

        if ((xmin > ymax) || (ymin > xmax))
            return false;

        if (ymin > xmin)
            xmin = ymin;
        if (ymax < xmax)
            xmax = ymax;

        zmin = (bounds[ray.sign[2]][2] - ray.origin[2]) * ray.invDirection[2];
        zmax = (bounds[1 - ray.sign[2]][2] - ray.origin[2]) * ray.invDirection[2];

        if ((xmin > zmax) || (zmin > xmax))
            return false;
        if (zmin > xmin)
            xmin = zmin;
        if (zmax < xmax)
            xmax = zmax;

        if ((xmin < ray.tMax) && (xmax > ray.tMin)) {
            box_tmax = xmax < ray.tMax ? xmax : ray.tMax;//min(xmax,ray.tmax);
            box_tmin = xmin > ray.tMin ? xmin : ray.tMin;//max(xmin,ray.tmin);
            return true;
        }

        else return false;
    }


    inline bool rayBoxIntersection(const Ray &ray) const {

        float xmin, xmax, ymin, ymax, zmin, zmax;
        Vector3 bounds[2] = {pMin, pMax};

        xmin = (bounds[ray.sign[0]][0] - ray.origin[0]) * ray.invDirection[0];
        xmax = (bounds[1 - ray.sign[0]][0] - ray.origin[0]) * ray.invDirection[0];
        ymin = (bounds[ray.sign[1]][1] - ray.origin[1]) * ray.invDirection[1];
        ymax = (bounds[1 - ray.sign[1]][1] - ray.origin[1]) * ray.invDirection[1];

        if ((xmin > ymax) || (ymin > xmax))
            return false;

        if (ymin > xmin)
            xmin = ymin;
        if (ymax < xmax)
            xmax = ymax;

        zmin = (bounds[ray.sign[2]][2] - ray.origin[2]) * ray.invDirection[2];
        zmax = (bounds[1 - ray.sign[2]][2] - ray.origin[2]) * ray.invDirection[2];

        if ((xmin > zmax) || (zmin > xmax))
            return false;
        if (zmin > xmin)
            xmin = zmin;
        if (zmax < xmax)
            xmax = zmax;

        if ((xmin < ray.tMax) && (xmax > ray.tMin)) {
            return true;
        }

        else return false;
    }


    inline friend std::ostream &operator<<(std::ostream &os, const BoundingBox &bb) {
        os << "pMin:" << bb.pMin << "  pMax:" << bb.pMax << std::endl;
        return os;
    }

} _MM_ALIGN16;


inline unsigned int partitionTriangles(unsigned int *&indexList, const Vector3 *const &centroids, const int dim,
                                       const double &pivot, const unsigned int left, const unsigned int right) {

    assert(right > left);

    unsigned int tmp;
    unsigned int i = left;
    unsigned int j = right;

    while (1) {

        while ((centroids[indexList[i]])[dim] < pivot) {
            //if(abs(centroids[indexList[i]][dim] - pivot)<EPS)
            //	std::cout<<"very close"<< std::endl;
            if (i == right) return right + 1; // special case: all tris on the left side
            i++;
        }

        while ((centroids[indexList[j]])[dim] > pivot) {
            //if(abs(centroids[indexList[j]][dim] - pivot)<EPS)
            //	std::cout<<"very close"<< std::endl;
            j--;
            if (j == left) break; // special case: all tris on the right
        }

        if (i >= j) return i;

        tmp = indexList[j];
        indexList[j] = indexList[i];
        indexList[i] = tmp;
        i++; // not j-- because then pivot element could not be recognized correctly by abort criterium in loop!
    }
}


inline unsigned int partitionTrianglesWithPivotAdjust(unsigned int *&indexList, const Vector3 *const &centroids,
                                                      const int dim, const double &pivot, const unsigned int left,
                                                      const unsigned int right) {

    assert(right > left);
    //std::cout << "proposed splitting plane: " << pivot;
    unsigned int swap;
    unsigned int i = left;
    unsigned int j = right;
    double pivotShift = pivot;
    double tmp;
    int count = 1;

    while (1) {

        while ((tmp = (centroids[indexList[i]])[dim]) < pivot) {
            pivotShift += tmp;
            count++;
            if (i == right)
                return partitionTriangles(indexList, centroids, dim, (pivotShift / count), left,
                                          right); // special case: all tris on the left side
            i++;
        }

        while ((tmp = (centroids[indexList[j]])[dim]) > pivot) {
            pivotShift += tmp;
            count++;
            j--;
            if (j == left) break; // special case: all tris on the right
        }

        if (i >= j)
            return partitionTriangles(indexList, centroids, dim, (pivotShift / count), left, right);

        swap = indexList[j];
        indexList[j] = indexList[i];
        indexList[i] = swap;
        i++; // not j-- because then pivot element could not be recognized correctly by abort criterium in loop!
    }
}


#endif /*BOUNDINGBOX_H_*/
