/*
 * NextEventEstimatorLight.cpp
 *
 *  Created on: Dec 23, 2008
 *      Author: bensteinert
 */

#include "NextEventEstimatorLight.h"
#include "Hitpoint.h"
#include "Sampler.h"


NextEventEstimatorLight::NextEventEstimatorLight(std::vector<Primitive *> ls) :
        primitives(0), cumDistributions(0), nrPrimitives(0) {

    totalArea = 0.0f;
    nrPrimitives = ls.size();
    std::cout << "Number of Light Source Primitives: " << nrPrimitives << std::endl;

    if (nrPrimitives == 0) {
        std::cout << "No light sources! You cant use DL estimation!";
        //TODO MIGRATION: indicator for DL possibility still needed?
        //DL = false;
    }
    else {
        //DL = true;
        std::cout << "Lights cool, DL possible" << std::endl;
        primitives = new Primitive *[nrPrimitives];
        cumDistributions = new float[nrPrimitives];

        for (int i = 0; i < nrPrimitives; i++) {
            primitives[i] = ls[i];
            totalArea += primitives[i]->area;
        }

        //CDF buildup:
        cumDistributions[0] = primitives[0]->area / totalArea;

        for (int i = 1; i < nrPrimitives; i++) {
            cumDistributions[i] = cumDistributions[i - 1] + primitives[i]->area / totalArea;
        }
    }
}


NextEventEstimatorLight::~NextEventEstimatorLight() {
    delete[] primitives;
}


void NextEventEstimatorLight::getNextEvent_unifdistr(Sampler &s, Hitpoint &nextHitEvent, float &inv_pd) const {
    Vector3 pos;
    Vector3 normal;

    double sample = s.genrand_real2();
    int index = 0;

    for (index = 0; index < nrPrimitives - 1 && sample > cumDistributions[index]; index++) {

    }

    primitives[index]->getunifdistrSample(s, pos, normal);
    inv_pd = totalArea;
    nextHitEvent = Hitpoint(pos, normal, primitives[index]);
}
