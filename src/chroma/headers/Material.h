#ifndef _MATERIAL_H_
#define _MATERIAL_H_


#include "Defines.h"
#include "Vector3.h"


class Hitpoint;


class Material {

public:

    Vector3 diffRefl;                //16 Byte
    Vector3 specRefl;                //16 Byte
    Vector3 transmission;            //16 Byte
    double diffCase;                //8 Byte Phong sampling probability
    double specCase;                //8 Byte Phong sampling probability
    float ior;                        //TODO:RAUS
    float powerEmitting;
    float n_u, n_v;
    unsigned char shaderID;
    unsigned char texShaderID;
    bool contribution;
    bool dispersive;
    float B1, B2, B3, C1, C2, C3;
    char name[16];

public:
    Material();

    Material(
            const Vector3 &diffRefl_in, const Vector3 &specRefl_in,
            const Vector3 &transm_in, const float ior_in, const float emitting,
            const float hardness, const unsigned char shader, const std::string &name_in);

    Material(float vals[12], const std::string &name);

    Material(const std::string &code);

    bool parse(const std::string &input);


private:
    void verify();

    bool initSellmeier(const std::string &code);

    void initSellmeierDefault();

    Vector3 RGBgetDiffRefl(const Hitpoint &hit) const;

    float SPEgetDiffRefl(const Hitpoint &hit, const float lambda) const;

    Vector3 RGBgetSpecRefl(const Hitpoint &hit) const;

    float SPEgetSpecRefl(const Hitpoint &hit, const float lambda) const;

    float SPEgetLe(const Hitpoint &hit, const float lambda) const;

    Vector3 RGBgetTransmission(const float dist) const;

    float SPEgetTransmission(const float dist, const float lambda) const;

    float SPEgetIOR(const float lambda) const;
};


inline float SPEgetUnpolFresnelTransmission(const float cosalpha, const float cosbeta, const float eta_from,
                                            const float eta_to) {
    // due to t+r=1 for dielectrica we only need to compute the t term...
    float cosbeta_mult_etato = eta_to * cosbeta;
    float cosaplha_mult_etafrom = eta_from * cosalpha;

    float ts = (2.0f * cosaplha_mult_etafrom) / (cosaplha_mult_etafrom + cosbeta_mult_etato);
    float tp = (2.0f * cosaplha_mult_etafrom) / (eta_to * cosalpha + eta_from * cosbeta);

    float result = 0.5f * (cosbeta_mult_etato / cosaplha_mult_etafrom) * (ts * ts + tp * tp);

    //std::cout << "transmission at alpha=" << acos(cosalpha) << " and beta=" << acos(cosbeta) << " check= " << asin(eta_from*sin(acos(cosalpha)/eta_to) ) << " -> " <<  result << std::endl;
    //DBG_ASSERT(result<1.0f);
    return result;
}


inline float SPEgetUnpolFresnelReflectance(const float cosalpha, const float cosbeta, const float eta_from,
                                           const float eta_to) {
    return 1.0f - SPEgetUnpolFresnelTransmission(cosalpha, cosbeta, eta_from, eta_to);
}


inline void changeReflection_VCoating(const float lambda, double &reflectionCoeff) {

    // 50nm steps from 350 to 800nm	   350, 400, 450, 500, 550, 600, 650, 700, 750, 800
    const static float VCoating[10] = {1.00, 0.80, 0.59, 0.42, 0.07, 0.19, 0.47, 0.71, 0.88,
                                       1.0};        // relative values to reduce reflection

    DBG_ASSERT(lambda >= 350.0f && lambda < 800.0f);

    const int bin = (int) (9.0f * (lambda - 350.0f) / (800.0f - 350.0f));
    DBG_ASSERT(bin > 0 && bin < 8);

    float a = VCoating[bin];
    float b = VCoating[bin + 1];
    float rest = lambda - (50 * bin + 350);
    DBG_ASSERT(rest >= 0.0f && rest < 50.0f);
    float result = a + ((b - a) * (rest / 50.0f));
    reflectionCoeff *= result;
}


inline void changeTransmission_VCoating(const float lambda, double &transmissionCoeff) {
    double reflectionCoeff = 1.0 - transmissionCoeff;
    changeReflection_VCoating(lambda, reflectionCoeff);
    transmissionCoeff = 1.0 - reflectionCoeff;
}


#endif