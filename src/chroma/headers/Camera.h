#ifndef CAMERA_H_
#define CAMERA_H_

#include <cfloat>

#include "Structs.h"
#include "Matrix3x3.h"
#include "Ray.h"
#include "Hitpoint.h"
#include "Aperture.h"
#include "AccumulationBuffer.h"
#include "NextEventEstimatorLens.h"

class Camera {

public:

    Vector3 pos;            // world coord pos
    Matrix3x3 rot;          // cam to world
    Matrix3x3 inv_rot;      // world to cam...
    Matrix3x3 save_rot;
    Vector3 save_pos;

    Aperture *lensSampler;
    NextEventEstimatorLens *neeLens;

    float sensitivity;
    float sensorZPos;            // local Z shift of sensor in mm!
    float focalDist;


    const int xRes;
    const int yRes;
    float sensorSizeX;      //mm!
    float sensorSizeY;      //mm!
    float sensorArea;       //mm²!!
    float pixelSizeX;       //mm!
    float pixelSizeY;       //mm!
    Vector3 shift;          // precalculated value to shift sensor plane from [-,+] to [0,+]

    Camera(const string &filename, const int xres, const int yres, const float sensitivity_in);

    virtual ~Camera();

    int LoadCamera(const string &filename);

    void loadCameraSettings(const float *const vals);

    virtual int evaluateRay(const Hitpoint &lensHit, Ray &lensRay, const float radiance, AccumulationBuffer &accBuff, ThreadEnv &sb) = 0;

    virtual int getRay(const float x, const float y, Ray &ray, float &Lweight, ThreadEnv &sb) = 0;

    virtual int getDebugRay(const float x, const float y, Ray &ray) = 0;

    virtual void stats(char *text1, char *text2, char *text3, char *text4, char *text5, const int length) = 0;

    virtual void focus(float dist) = 0;

    virtual void zoomRel(float mult) = 0;

    virtual void zoomAbs(float fl) = 0;

    virtual void stopUp() = 0;

    virtual void stopDown() = 0;

    virtual void setStop(const float apertureRadius) = 0;

    void rotate(const int &deg_x, const int &deg_y);

    void move(const Vector3 &move);

    void reset();

    void superSample(const float scaling);

    Vector3 getPixelCoord(const float x, const float y, const float shift_x, const float shift_y);

    void zShift(float z);

    void toWorldCoordSys(Ray &ray);

    void toCamCoordSys(Ray &ray);

    Ray rayInCamCoords(const Vector3 &origin, const Vector3 &dir, const float lambda);

    Ray rayInWorldCoords(const Vector3 &origin, const Vector3 &dir, const float lambda);

    Vector3 dirInWorldCoords(const Vector3 &dir);

    Vector3 dirInCamCoords(const Vector3 &dir);

    Vector3 pointInWorldCoords(const Vector3 &point);

    Vector3 pointInCamCoords(const Vector3 &point);

    int sensorHit(const Vector3 &sensorPlanePoint, const int resX, const int resY);

    friend std::ostream & operator<<(std::ostream &os, const Camera &camera);

};

#endif
