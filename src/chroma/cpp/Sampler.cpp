/*
 * Sampler.cpp
 *
 *  Created on: Dec 11, 2008
 *      Author: bensteinert
 */

#include "Chroma.h"
#include "Hitpoint.h"
#include "Sampler.h"

Sampler::Sampler() { }


Sampler::~Sampler() { }


void Sampler::init(unsigned long seed) {
    mt.init_genrand(seed);
}

double Sampler::genrand_real2() {
    return mt.genrand_real2();
}

double Sampler::genrand_real3() {
    return mt.genrand_real3();
}


inline int Sampler::terminate(float rr_limit) {
    return mt.genrand_real2() < rr_limit;
}


Vector3 Sampler::get_unifdistr_DiskSample() {
    float u = mt.genrand_real2();
    float v = mt.genrand_real2();
    float sqrtu = sqrtf(u);
    float v2pi = v * TWO_PI_f;
    return Vector3(sqrtu * cosf(v2pi), sqrtu * sinf(v2pi), 0.0f);
}


Vector3 Sampler::get_unifdistr_HemSample(const Hitpoint &hit) {
    float u = (float) mt.genrand_real2();
    float v = (float) mt.genrand_real2();
    float sqrtone_u = sqrtf(1.0f - u * u);
    float v2pi = v * TWO_PI_f;
    Vector3 unitSampleDir = Vector3(sqrtone_u * cosf(v2pi), sqrtone_u * sinf(v2pi), u);
    return (unitSampleDir[0] * hit.t1 + unitSampleDir[1] * hit.t2 + unitSampleDir[2] * hit.n).normalizedCopy();
}


Vector3 Sampler::get_unifdistr_UnitHemSample() {
    float u = (float) mt.genrand_real2();
    float v = (float) mt.genrand_real2();
    float sqrtone_u = sqrtf(1.0f - u * u);
    float v2pi = v * TWO_PI_f;
    return Vector3(sqrtone_u * cosf(v2pi), sqrtone_u * sinf(v2pi), u);
}


Vector3 Sampler::get_unifdistr_CapSample(float cos_theta_max) {
    /*
     * from Global Illumination Compendium p19f
     * pdf is 1/(2Pi*(1-costhetamax))
     */

    float u = (float) mt.genrand_real2();
    float v = (float) mt.genrand_real2();
    float sqrtTerm = sqrtf(1.0f - powf(1.0f - u * (1.0f - cos_theta_max), 2.0f));
    float v2pi = v * TWO_PI_f;

    Vector3 unitSampleDir = Vector3(sqrtTerm * cosf(v2pi), sqrtTerm * sinf(v2pi), 1.0f - u * (1.0f - cos_theta_max));
    return unitSampleDir;
}


Vector3 Sampler::get_cosdistr_CapSample(float cos_theta_max) {
    /*
     * from Global Illumination Compendium p19f
     * pdf is costheta/(PI*sinsin(thetamax))
     */

    float u = (float) mt.genrand_real2();
    float v = (float) mt.genrand_real2();

    float v2pi = v * TWO_PI_f;
    float sqrtu = sqrt(u);
    float sin_theta_max = sqrt(1.0f - cos_theta_max * cos_theta_max);
    Vector3 unitSampleDir = Vector3(sqrtu * cosf(v2pi) * sin_theta_max, sqrtu * sinf(v2pi) * sin_theta_max,
                                    sqrt(1.0f - u * sin_theta_max * sin_theta_max));
    return unitSampleDir;
}


Vector3 Sampler::get_cosdistr_HemSample(const Hitpoint &hit) {
    float u = mt.genrand_real2();
    float v = mt.genrand_real2();
    float sqrtu = sqrt(u);
    float v2pi = v * TWO_PI_f;

    Vector3 unitSampleDir = Vector3(cos(v2pi) * sqrtu, sin(v2pi) * sqrtu, sqrt(1.0f - u));
    return (unitSampleDir[0] * hit.t1 + unitSampleDir[1] * hit.t2 + unitSampleDir[2] * hit.n).normalizedCopy();
}


Vector3 Sampler::get_cosdistr_LobeSample(Vector3 &dir, int n) {

    Vector3 t1, t2;
    getCoordSystem(dir, t1, t2);
    float u = mt.genrand_real2();
    float v = mt.genrand_real2();

    float temp = sqrt(1.0f - pow(u, (2.0f / (n + 1))));
    float v2pi = v * TWO_PI_f;

    // now sample along normal with wished lobe. Rotate with angles of mirror_dir an make cartesian dir
    Vector3 unitSampleDir = Vector3(cos(v2pi) * temp, sin(v2pi) * temp, pow(u, 1.0f / (n + 1)));
    return (unitSampleDir[0] * t1 + unitSampleDir[1] * t2 + unitSampleDir[2] * dir).normalizedCopy();

}


Vector3 Sampler::get_spec_AshiSample(const Hitpoint &hit, const Vector3 &inv_out_dir, const int &n_u, const int &n_v,
                                   Vector3 &halfVector, double &inv_pd) {
    //inv_out_dir shows in direction of hitpoint!

    retry:
    double u = mt.genrand_real2();
    double v = mt.genrand_real2();
    double phi = atan(sqrt((n_u + 1.0) / (n_v + 1.0)) * tan((PI * u) / 2.0));
    double theta = acos(pow(1.0 - v, (1.0 / (n_u * pow(cos(phi), 2.0) + n_v * pow(sin(phi), 2.0) + 1.0))));

    int sector = (int) (mt.genrand_real2() * 4.0); //select phi-sector
    phi = phi + ((double) sector) * HALF_PI_f;

    halfVector = Vector3(
            (cos(phi) * sin(theta) * hit.t1) + (sin(phi) * sin(theta) * hit.t2) + (cos(theta) * hit.n));
    halfVector.normalize();
    Vector3 inc_dir = mirror(inv_out_dir, halfVector);
    float cos_theta_i = inc_dir * hit.n;
    if (cos_theta_i < 0.0f)
        goto retry; // restart if sample direction lies beneath surface


    inv_pd = 1.0 / ((sqrt((n_u + 1.0) * (n_v + 1.0)) / (2.0 * PI) * pow((double) (hit.n * halfVector),
                                                                        ((double) n_u * pow(cos(phi), 2.0) +
                                                                         (double) n_v * pow(sin(phi), 2.0)))) /
                    (4.0 * PI * (inc_dir * halfVector)));
    return inc_dir;
}


/*
inline Vector3 ConcentricSampleDisk(float u1, float u2,
        float *dx, float *dy) {
    float r, theta;
    // Map uniform random numbers to $[-1,1]^2$
    float sx = 2 * u1 - 1;
    float sy = 2 * u2 - 1;
    // Map square to $(r,\theta)$
    // Handle degeneracy at the origin
    if (sx == 0.0 && sy == 0.0) {
        *dx = 0.0;
        *dy = 0.0;
        return;
    }
    if (sx >= -sy) {
        if (sx > sy) {
            // Handle first region of disk
            r = sx;
            if (sy > 0.0)
                theta = sy/r;
            else
                theta = 8.0f + sy/r;
        }
        else {
            // Handle second region of disk
            r = sy;
            theta = 2.0f - sx/r;
        }
    }
    else {
        if (sx <= sy) {
            // Handle third region of disk
            r = -sx;
            theta = 4.0f - sy/r;
        }
        else {
            // Handle fourth region of disk
            r = -sy;
            theta = 6.0f + sx/r;
        }
    }
    theta *= M_PI / 4.f;
    *dx = r*cosf(theta);
    *dy = r*sinf(theta);
}
*/
