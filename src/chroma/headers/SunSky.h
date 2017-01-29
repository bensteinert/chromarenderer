/*
 * SunSky.h
 *
 *  Created on: Jan 27, 2009
 *      Author: bensteinert
 */

#ifndef SUNSKY_H_
#define SUNSKY_H_

#include "Defines.h"
#include "Environment.h"
#include "Vector3.h"

class SunSky: public Environment{

public:
	bool overcast;
	float thetaSunInRad;
	float turbidity;
	Vector3 sun_XYZ;
	Vector3 sun_rgb;
	Vector3 sunDir;
	//Vector3 u;
	//Vector3 v;
	float sun_power_scale;
	Vector3 zenith_xyY;
	float perez_x[5];
	float perez_y[5];
	float perez_Y[5];
	//float sun_M1;
	//float sun_M2;

	SunSky(const float timeOfDay);
	void Init();
	virtual ~SunSky();
	virtual void getEnvironmentLight(const Ray& ray, float& radiance)const;
	//void getOvercastSkylight(const Vector3& dir, Vector3& radiance) const;
};

#endif /* SUNSKY_H_ */
