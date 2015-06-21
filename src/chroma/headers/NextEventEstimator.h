#ifndef NEXTEVENTESTIMATOR_H_
#define NEXTEVENTESTIMATOR_H_

#include <vector>

class Hitpoint;
class Sampler;
class NextEventEstimator {

public:
    float totalArea;

    NextEventEstimator() : totalArea(0.0f) { };

    virtual ~NextEventEstimator() { };
    virtual void getNextEvent_unifdistr(Sampler &s, Hitpoint &nextHitEvent, float &inv_pd) const = 0;
};

#endif /*NEXTEVENTESTIMATOR_H_*/
