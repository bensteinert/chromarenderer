#include <iostream>

#include "Chroma.h"
#include "Ray.h"
#include "Sphere.h"
#include "Hitpoint.h"
#include "BoundingBox.h"
#include "Sampler.h"


Sphere::Sphere() :
        Primitive(&stdDiffuse, true), center(Vector3(0, 0, 0)), radius(0.0f) {
    area = 0.0f;
}


Sphere::Sphere(float coords[4], Material *mat_in) :
        Primitive(mat_in, true), center(coords), radius(coords[3]) {
    area = getArea();
}


Sphere::Sphere(const Vector3 &c, const float rad, Material *mat_in) :
        Primitive(mat_in, true), center(c), radius(rad) {
    area = getArea();
}


Sphere::~Sphere() { }


void Sphere::getBoundingBox(BoundingBox &bb) const {
    Vector3 extent = Vector3(radius, radius, radius);;
    bb.pMin = center - extent;
    bb.pMax = center + extent;
}


Vector3 Sphere::getNormal(const Vector3 &pos, const Vector3 &dir) const {
    // dir and normal are to show in the same direction!
    Vector3 n = (pos - center).normalizedCopy();
    return n * ieeeSignWOif(n * dir);
}


void Sphere::getNormal(const Vector3 &dir, Hitpoint &hit) const {
    // dir is reversed ray direction in general!
    Vector3 n = (hit.p - center).normalizedCopy();
    if (n * dir > 0.0f) {
        hit.n = n;
        hit.flipped = false;
    }
    else {
        hit.n = -n;
        hit.flipped = true;
    }
}


float Sphere::getArea() const {
    return 4.0f * PI_f * radius * radius;
}


float Sphere::intersectRay(const Ray *ray, float &u, float &v) const {
    //code from http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection

    //Compute A, B and C coefficients
    Vector3 OminusC = ray->origin - center;
    double a = HPrecDot(ray->direction, ray->direction);
    double b = HPrecDot(ray->direction, OminusC) * 2.0;
    double c = HPrecDot(OminusC, OminusC) - ((double) radius * (double) radius);

    //Find discriminant
    double disc = (b * b) - (4.0 * a * c);

    // if discriminant is negative there are no real roots, so return
    // false as ray misses sphere
    if (disc < 0.0) return 0.0f;

    // compute q as described above
    double distSqrt = sqrt(disc);
    double q;
    q = b < 0 ? (-b - distSqrt) / 2.0 : (-b + distSqrt) / 2.0;

    // compute t0 and t1
    float t0 = q / a;
    float t1 = c / q;

    // make sure t0 is smaller than t1
    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // ignore imprecision flaws.....
    if (!(t0 < NaNLIMIT) || !(t1 < NaNLIMIT)) {
        std::cout << "NAN" << std::endl;
        return 0.0f;
    }

    // if t1 is less than tmin, the object is in the ray's negative direction
    // and consequently the ray misses the sphere
    if (t1 < ray->tMin + EPS)
        return 0.0f;

    // if t0 is less than zero, the intersection point is at t1
    if (t0 < ray->tMin + EPS)
        //if (t0 < 0.05f)
        return t1;
        // else the intersection point is at t0
    else
        return t0;
    // return t with ray interval because 2nd hitpint could be interesting!
}


int Sphere::intersectRay(const Ray *ray, Hitpoint &hit) const {
    std::cerr << "Sphere::intersectRay(const Ray *ray, Hitpoint& hit, float& u, float& v) not yet implemented!" <<
    std::endl;
    return 0;
}


int Sphere::intersectRay2(const Ray *ray, float &t1, float &t2) const {
    //code from http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection
    //special intersect routine returning both t values for later analysis eg in lens intersection test

    //Compute A, B and C coefficients
    Vector3 OminusC = ray->origin - center;
    double a = HPrecDot(ray->direction, ray->direction);
    double b = HPrecDot(ray->direction, (OminusC)) * 2.0;
    double c = HPrecDot(OminusC, OminusC) - ((double) radius * (double) radius);
//	float a = ray->direction*ray->direction;
//	float b = ray->direction*(OminusC)*2.0f;
//	float c = (OminusC*OminusC) - (radius * radius);

    //Find discriminant
    double disc = (b * b) - (4.0 * a * c);

    // if discriminant is negative there are no real roots, so return
    // false as ray misses sphere
    if (disc < 0.0) {
        t1 = 0.0f;
        t2 = 0.0f;
        return 0;
    };

    // compute q as described above
    double distSqrt = sqrt(disc);
    double q;
    q = b < 0 ? (-b - distSqrt) / 2.0 : (-b + distSqrt) / 2.0;

    // compute t0 and t1
    t1 = q / a;
    t2 = c / q;

    if (t1 > t2) {
        float temp = t1;
        t1 = t2;
        t2 = temp;
    }

    return 1;
}


float Sphere::intersectSphere(const Sphere &s) const {

    // compute everything beside original values due to better calculation possibilities
    // virtual center of this: 0,0,0
    // virtual center or s: dist,0,0

    float dist = (s.center - center).length();

    // the intersection of two spheres is a circle with radius result
    // the following formula origins Wolfram Mathworld: Chapter Sphere-Sphere Intersection


    if (dist < EPS) {
        if (radius == s.radius) {
            // if spheres overlap completely the overlap circle has the maximum radius
            // extra case needed, because 1/dist would be INF
            return radius;
        }
        else {
            // one sphere lies completely in the other one.
            return 0.0f;
        }
    }

    float distSq = dist * dist;
    float radiusS1Sq = radius * radius;
    float radiusS2Sq = s.radius * s.radius;

    float sqrtTerm = (4.0f * distSq * radiusS1Sq) - powf(distSq - radiusS2Sq + radiusS1Sq, 2.0f);

    if (sqrtTerm <= 0.0f)
        // case that spheres do not intersect (tangential case means no intersection!)
        return 0.0f;

    float result = (1.0f / (2.0f * dist)) * sqrtf(sqrtTerm);

    return 2.0f * result;

}


float Sphere::getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const {

    double u = (s.genrand_real3() - 0.5) * 2.0;
    double v = s.genrand_real2();
    double sqrtone_u = sqrt(1.0 - u * u); // sin² + cos² = 1 -> sin = sqrt(1-cos²)
    float v2pi = v * TWO_PI_f;

    position = Vector3(sqrtone_u * cosf(v2pi), sqrtone_u * sinf(v2pi), u);
    normal = (position - center).normalizedCopy();
    return area;
}
