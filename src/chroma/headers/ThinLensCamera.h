#ifndef THINLENSCAMERA_H_
#define THINLENSCAMERA_H_

#include <cfloat>
#include <string>

#include "Matrix3x3.h"
#include "Ray.h"
#include "Hitpoint.h"
#include "Aperture.h"
#include "AccumulationBuffer.h"
#include "Camera.h"


class ThinLensCamera : public Camera {

public:

    Aperture ap;

    ThinLensCamera(const string &filename, const int xres, const int yres, float sensitivity_in);

    ~ThinLensCamera();

    int getRay(const float x, const float y, Ray &ray, float &Lweight, ThreadEnv &sb);

    int getDebugRay(const float x, const float y, Ray &ray);

    int evaluateRay(const Hitpoint &lensHit, Ray &lensRay, const float radiance, AccumulationBuffer &accBuff, ThreadEnv &sb);

    void updateSampler();

    void focus(float dist);

    void stats(char *text1, char *text2, char *text3, char *text4, char *text5, const int length);

    void zoomAbs(float fl);

    void zoomRel(float mult);

    void stopUp();

    void stopDown();

    void setStopNumber(const float stopNumber);

    void * operator new(size_t bytes);

    float getLensSampleLocal(Sampler &s, Vector3 &sample);

} _MM_ALIGN16;

#endif /*THINLENSCAMERA_H_*/
