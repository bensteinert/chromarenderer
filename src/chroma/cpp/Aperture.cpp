#include "Disk.h"
#include "Triangle.h"
#include "Aperture.h"
#include "Hitpoint.h"
#include "Chroma.h"
#include "Ray.h"
#include "BoundingBox.h"



Aperture::Aperture() :
        Primitive(&stdAir, false, &stdAir), circular(true), radius(0.0f), numTris(0), tris(0), objectID(APID) { }


Aperture::Aperture(const Aperture &ap_in) :
        Primitive(&stdAir, false, &stdAir), circular(ap_in.circular), radius(ap_in.radius),
        numTris(ap_in.numTris), tris(0), center(ap_in.center), normal(ap_in.normal), objectID(APID) {
    if (numTris > 0) {
        tris = new Triangle[numTris];
        for (int i = 0; i < numTris; i++) {
            tris[i] = ap_in.tris[i];
        }
    }
    else {
        circAP = Disk(center, normal, radius, &stdAir, &stdDiffuse);
    }
}


Aperture::Aperture(int numBlades, const Vector3 &center_in, float radius_in) :
        Primitive(&stdAir, false, &stdAir), circular(true), radius(0.0f), numTris(0), tris(0), objectID(APID) {
    init(numBlades, center_in, radius_in);
}


void Aperture::init(int numBlades, const Vector3 &center_in, float radius_in) {
    radius = radius_in;
    center = center_in;
    numTris = numBlades;

    if (numBlades == 0) {
        //circular
        circAP = Disk(center, Vector3(0, 0, -1), radius, &stdAir, &stdDiffuse);
        normal = circAP.normal;
        return;
    }

    circular = false;
    tris = new Triangle[numTris];

    if (numTris > 2) {
        double angleStep = TWO_PI / numBlades;
        double angle = 0;

        for (int i = 0; i < numTris; i++) {
            tris[i] = Triangle(center, Vector3(cos(angle + angleStep), sin(angle + angleStep), 0) * radius + center,
                               Vector3(cos(angle), sin(angle), 0) * radius + center, &stdAir, &stdAir);
            angle += angleStep;
        }

        assert(equals(angle, TWO_PI_f));

//        for (int i = 0; i < numTris - 1; i++) {
//            assert(equals(tris[i].n, tris[i + 1].n));
//        }

        normal = tris[0].n;
        area = getArea();
    }
    else {
//		if(numBlades==1){
//			// halfplane approx
//			tris[0] = Triangle(Vector3(-1,0,0)*radius+center,Vector3(0,1,0)*radius+center,Vector3(0,-1,0)*radius+center,&stdAir,&stdAir);
//			area = getArea();
//			normal = tris[0].n;
//		}
//		else if (numBlades==2){
//			//slit
//			float angle = atan(20.0f);	// tan_alpha = radius/(radius/10)
//			tris[0] = Triangle(Vector3(-radius/20.0f,0,0)+center,Vector3(cos(angle),sin(angle),0)*radius+center,Vector3(cos(-angle),sin(-angle),0)*radius+center,&stdAir,&stdAir);
//			tris[1] = Triangle(Vector3(radius/20.0f,0,0)+center,Vector3(-cos(angle),-sin(angle),0)*radius+center,Vector3(-cos(-angle),-sin(-angle),0)*radius+center,&stdAir,&stdAir);
//			area = getArea();
//
//			assert(equals(tris[0].n,tris[1].n));
//			normal = tris[0].n;
//		}
    }
}


Aperture::Aperture(const int numVerts, const Vector3 *vertices, const float radius_in) : numTris(numVerts) {

    circular = false;

    for (int i = 0; i < numVerts; i++) {
        center += vertices[i];
    }

    center = center / numVerts;

    tris = new Triangle[numVerts];

    for (int i = 0; i < numTris; i++) {
        tris[i] = Triangle(center, vertices[i], vertices[(i + 1) % numVerts], &stdNull, &stdAir);
    }

//    for (int i = 0; i < numTris - 1; i++) {
//        assert(equals(tris[i].n, tris[i + 1].n));
//    }

    normal = tris[0].n;
    radius = (vertices[0] - center).length();
    float scale = radius_in / radius;
    scaleToRadius(scale);
    area = getArea();
}


Aperture::~Aperture() {
    delete[] tris;
}


void Aperture::getBoundingBox(BoundingBox &bb) const {
    BoundingBox tmpBB;

    if (circular) {
        circAP.getBoundingBox(bb);
        return;
    }

    for (int i = 0; i < numTris; i++) {
        tris[i].getBoundingBox(tmpBB);
        bb.merge(tmpBB);
    }
}


float Aperture::getArea() const {
    if (circular) {
        return circAP.area;
    }

    float result = 0.0f;
    for (int i = 0; i < numTris; i++) {
        result += tris[i].getArea();
    }
    return result;
}


float Aperture::intersectRay(const Ray *ray, float &u, float &v) const {
    //used by AccStruct only

    if (circular) {
        return circAP.intersectRay(ray, u, v);
    }

    float result;

    for (int i = 0; i < numTris; i++) {
        if ((result = tris[i].intersectRay(ray, u, v)) > 0.0f) {
            return result;
        }
    }

    return 0.0f;
}


int Aperture::intersectRay(const Ray *ray, Hitpoint &hit) const {
    //general purpose use

    if (circular) {
        return circAP.intersectRay(ray, hit) > 0.0f ? 2 : 0;

    }

    float t = 0.0f;
    for (int i = 0; i < numTris; i++) {
        if ((t = tris[i].intersectRay(ray, hit.u, hit.v)) > 0.0f) {
            hit.dist = t;
            hit.hit = true;
            hit.p = ray->at(t);
            getNormal(ray->direction, hit);
            hit.index = &(tris[i]);
            return 2;            // important that LensTracer notices stop!
        }
    }

    return 0;
}


float Aperture::getunifdistrSample(Sampler &s, Vector3 &position) const {

    if (circular) {
        return circAP.getunifdistrSample(s, position);
    }

    // sample triangle and then point on it
    int randTris = s.genrand_real2() * numTris;
    tris[randTris].getunifdistrSample(s, position);
    return area;
}


float Aperture::getunifdistrSample(Sampler &s, Vector3 &position, Vector3 &normal) const {
    if (circular) {
        return circAP.getunifdistrSample(s, position, normal);
    }

    // sample triangle and then point on it
    int randTris = s.genrand_real2() * numTris;
    tris[randTris].getunifdistrSample(s, position, normal);
    return area;
}


float Aperture::getunifdistrSample(Sampler &s, const Pupil &pupilData, Vector3 &position, Vector3 &normal_out) const {

    assert(circular);
    Vector3 t1, t2;
    getCoordSystem(normal, t1, t2);

    position = s.get_unifdistr_DiskSample();
    position *= pupilData.radius;                                        // scale to radius
    position = (position[0] * t1 + position[1] * t2 + position[2] * normal);    // rotate correctly
    position = position + pupilData.center;

    normal_out = normal;                        // normal shows outside by default!

    return area;
}


float Aperture::getunifdistrSamples(Sampler &s, const int nrSamples, Vector3 *&samples) const {

    if (circular) {
        return circAP.getunifdistrSamples(s, nrSamples, samples);
    }

    Vector3 t1, t2;
    getCoordSystem(normal, t1, t2);
    samples = new Vector3[nrSamples];

    for (int i = 0; i < nrSamples; i++) {
        Vector3 sample;
        int randTris = s.genrand_real2() * numTris;
        tris[randTris].getunifdistrSample(s, sample);
        samples[i] = (sample[0] * t1 + sample[1] * t2 + sample[2] * normal) + center;
    }
    return area;
}


Vector3 Aperture::getNormal(const Vector3 &pos, const Vector3 &dir) const {
    // dir must show in same direction like normal
    return normal * ieeeSignWOif(normal * dir);
}


void Aperture::getNormal(const Vector3 &dir, Hitpoint &hit) const {
    // dir must show in same direction like normal
    if (normal * dir > 0.0f) {
        hit.n = normal;
        hit.flipped = false;
    }
    else {
        hit.n = -normal;
        hit.flipped = true;
    }
}


//TODO needed for camera movement?
void Aperture::transform(const Vector3 &translation, const float &scale, const Matrix3x3 &rotation) {

    if (circular) {
        circAP.transform(translation, scale, rotation);
    }
    else {
        for (int i = 0; i < numTris; i++) {
            tris[i].transform(translation, scale, rotation);
        }
    }
}


void Aperture::out(std::ostream &os) const {
    os << "Center: " << center << std::endl << "Radius: " << radius << std::endl <<
    "#Tris: " << numTris << std::endl << "Area: " << area << std::endl;

    for (int i = 0; i < numTris; i++) {
        tris[i].out(os);
    }
}


void Aperture::shift(const Vector3 &v) {

    if (circular) circAP.shift(v);
    else {
        for (int i = 0; i < numTris; i++) {
            tris[i].p0 += v;
            tris[i].p1 += v;
            tris[i].p2 += v;
        }
    }
    center += v;
}


void Aperture::scaleRel(float scale) {

    if (circular) {
        circAP.scaleRel(scale);
    }
    else {
        for (int i = 0; i < numTris; i++) {
            tris[i].p0 *= scale;
            tris[i].p1 *= scale;
            tris[i].p2 *= scale;
        }
    }

    center *= scale;
    radius *= scale;
    area = getArea();
}


void Aperture::scaleAbs(float val) {
    float scale = val / radius;    // /radius for normalizing * scale for transformation
    scaleRel(scale);
}


void Aperture::stopUp() {
    scaleToRadius(radius / SQRT2);
}


void Aperture::stopDown() {
    scaleToRadius(radius * SQRT2);
}


void inline Aperture::scaleToRadius(float rad) {
    if (circular) {
        circAP.scaleToRadius(rad);
    }
    else {
        float scale = rad / radius;    ///radius for normalizing * scale for transformation
        for (int i = 0; i < numTris; i++) {
            tris[i].p0 = (tris[i].p0 - center) * scale + center;
            tris[i].p1 = (tris[i].p1 - center) * scale + center;
            tris[i].p2 = (tris[i].p2 - center) * scale + center;
        }
    }

    radius = rad;
    area = getArea();
}