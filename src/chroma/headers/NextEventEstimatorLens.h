/*
 * NextEventEstimatorLens.h
 *
 *  Created on: Dec 23, 2008
 *      Author: bensteinert
 */

#ifndef NEXTEVENTESTIMATORLENS_H_
#define NEXTEVENTESTIMATORLENS_H_

#include "NextEventEstimator.h"
#include "Primitive.h"

class NextEventEstimatorLens: public NextEventEstimator{

	Primitive *lens;

public:
	NextEventEstimatorLens(Primitive* lens_in);
	virtual ~NextEventEstimatorLens();
	virtual void getNextEvent_unifdistr(Sampler& s, Hitpoint& nextHitEvent, float& inv_pd) const;
};


#endif /* NEXTEVENTESTIMATORLENS_H_ */
