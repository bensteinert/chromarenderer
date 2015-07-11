/*
 * NextEventEstimatorLens.cpp
 *
 *  Created on: Dec 23, 2008
 *      Author: bensteinert
 */

#include "NextEventEstimatorLens.h"
#include "Hitpoint.h"


NextEventEstimatorLens::NextEventEstimatorLens(Primitive *lens_in) : lens(lens_in) {
    totalArea = lens->area;
}


NextEventEstimatorLens::~NextEventEstimatorLens() { }


void NextEventEstimatorLens::getNextEvent_unifdistr(Sampler &s, Hitpoint &nextHitEvent, float &inv_pd) const {
    Vector3 pos;
    Vector3 normal;

    inv_pd = lens->getunifdistrSample(s, pos, normal);    // lens sampler works with global coord object!
    nextHitEvent = Hitpoint(pos, normal, lens);
}
