#ifndef SAMPLER_H_
#define SAMPLER_H_

#include "mtrand.h"

class Vector3;
class Hitpoint;


class Sampler {

    //dSFMT mt;
    //double rand[65536];
    MT::MersenneTwist mt;

public:

    Sampler();

    ~Sampler();

    void init(unsigned long seed);

    double genrand_real2();

    double genrand_real3();

    Vector3 get_unifdistr_DiskSample();

    int terminate(float rr_limit);

    Vector3 get_unifdistr_HemSample(const Hitpoint &hit);

    Vector3 get_unifdistr_UnitHemSample();

    Vector3 get_unifdistr_CapSample(float cos_theta_max);

    Vector3 get_cosdistr_CapSample(float cos_theta_max);

    Vector3 get_cosdistr_HemSample(const Hitpoint &hit);

    Vector3 get_cosdistr_LobeSample(Vector3 &dir, int n);

    Vector3 get_spec_AshiSample(const Hitpoint &hit, const Vector3 &inv_out_dir, const int &n_u, const int &n_v,
                                Vector3 &halfVector, double &inv_pd);
};

#endif /* SAMPLER_H_ */
