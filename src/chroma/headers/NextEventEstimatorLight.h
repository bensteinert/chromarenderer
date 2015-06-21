#ifndef NEXTEVENTESTIMATORLIGHT_H_
#define NEXTEVENTESTIMATORLIGHT_H_

#include <vector>
#include "NextEventEstimator.h"
#include "Primitive.h"

class NextEventEstimatorLight: public NextEventEstimator{

	Primitive **primitives;
	float *cumDistributions;
	unsigned int nrPrimitives;

public:
	NextEventEstimatorLight(std::vector<Primitive*> ls);
	virtual ~NextEventEstimatorLight();
	virtual void getNextEvent_unifdistr(Sampler& s, Hitpoint& nextHitEvent, float& inv_pd) const;

};

float getD65Lambda(const float& rand);


#endif /* NEXTEVENTESTIMATORLIGHT_H_ */
