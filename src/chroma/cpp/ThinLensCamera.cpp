#include <cassert>
#include <mm_malloc.h>

#include "ThinLensCamera.h"
#include "ChromaCIE.h"
#include "Chroma.h"


ThinLensCamera::ThinLensCamera(const string &filename, const int xres, const int yres, float sensitivity_in) :
        Camera(filename, xres, yres, sensitivity_in), ap(Aperture(6, Vector3(0, 0, 0), focalDist / (8.0f * 2.0f))) {
    
    zoomAbs(focalDist);
    sensorZPos = focalDist + 0.1f;                                    // focus at infinity
    updateSampler();
}


ThinLensCamera::~ThinLensCamera() { }


void ThinLensCamera::updateSampler() {
    delete lensSampler;
    lensSampler = new Aperture(ap);
    lensSampler->insideMat = &lightSensing;
    lensSampler->transform(pos, 0.001, rot);
    delete neeLens;
    neeLens = new NextEventEstimatorLens(lensSampler);
}


int ThinLensCamera::getRay(const float x, const float y, Ray &ray, float &Lweight, ThreadEnv &sb) {
    const float shift_x = sb.sampler.genrand_real2();
    const float shift_y = sb.sampler.genrand_real2();
    Vector3 pixelPos = getPixelCoord(x, y, shift_x, shift_y);
    Vector3 principalDir = -(pixelPos.normalizedCopy());
    float cos_theta = -principalDir[2];
    int arrayPos = y * xRes + x;
    float AAG = sensorArea;
    float g =
            (sensorZPos * focalDist) / (focalDist - sensorZPos);        // construct focalPoint at g through lens center
    Vector3 focalPointObjectside = principalDir * (-g / cos_theta);
    Vector3 lensSample;
    float inv_pd = getLensSampleLocal(sb.sampler, lensSample);                    //mm²
    Vector3 dirToLens = (lensSample - pixelPos).normalizedCopy();
    Vector3 localDir = (focalPointObjectside - lensSample).normalizedCopy();
    float cos4 = powf(dirToLens[2], 4.0f);

    // todo: start with Bayer Pattern here?
    ray = rayInWorldCoords(lensSample, localDir, spectrum_sample(sb.sampler.genrand_real2()));    
    AAG = AAG * inv_pd * (cos4 / (sensorZPos * sensorZPos));

    Lweight = sensitivity * AAG;    // /100000 left out! we are measuring in Watt/mm²!!!
    return arrayPos;
}

float ThinLensCamera::getLensSampleLocal(Sampler &s, Vector3 &sample) {
    // lens sample point in local coordinates
    ap.getunifdistrSample(s, sample);
    return ap.area;
}


int ThinLensCamera::getDebugRay(const float x, const float y, Ray &ray) {
    Vector3 pixelPos = getPixelCoord(x, y, 0.5f, 0.5f);
    int arrayPos = y * xRes + x;
    Vector3 principalDir = -(pixelPos.normalizedCopy());
    float cos_theta = -principalDir[2];
    float g =
            (sensorZPos * focalDist) / (focalDist - sensorZPos);        // construct focalPoint at g through lens center
    Vector3 focalPointObjectside = principalDir * (-g / cos_theta);
    Vector3 lensSample = Vector3(0, 0, 0);
    //Vector3 dirToLens = (lensSample - pixelPos).normalizedCopy();
    Vector3 localDir = (focalPointObjectside - lensSample).normalizedCopy();

    ray = rayInWorldCoords(lensSample, localDir, 555);
    return arrayPos;
}


int ThinLensCamera::evaluateRay(const Hitpoint &lensHit, Ray &lensRay, const float radiance,
                                AccumulationBuffer &accBuff, ThreadEnv &sb) {

    //lokale Kamera sieht auch entlang der negativen z-achse!
    Vector3 scenePoint_CC = pointInCamCoords(lensRay.origin);
    Vector3 lensPoint_CC = pointInCamCoords(lensHit.p);

    float z = focalDist / (scenePoint_CC[2] - focalDist);       // multiplicator for projecting point to image plane
    Vector3 imagePoint = z * scenePoint_CC;                     // point of focus is direction for the ray after lens

    Vector3 refracted_lightRay_CC = (imagePoint - lensPoint_CC).normalizedCopy();   // ray after lens by connect of image point and lensSample
    float t = sensorZPos / refracted_lightRay_CC[2];                                // length of vector after lens
    Vector3 filmHit_CC = lensPoint_CC + t * refracted_lightRay_CC;                  // full: COP + t*v
    filmHit_CC = filmHit_CC + shift;                                                // shift to avoid negative values in pixel finding

    // cull ray against sensor area
    if (filmHit_CC[0] > 0.0f && filmHit_CC[0] < sensorSizeX && filmHit_CC[1] > 0.0f && filmHit_CC[1] < sensorSizeY) {

        int pixelPos = ((int) (filmHit_CC[1] / pixelSizeY)) * (accBuff.width) +
                       ((accBuff.width - 1) - (int) (filmHit_CC[0] / pixelSizeX)); // pixel position on the sensor
        DBG_ASSERT(pixelPos < accBuff.width * accBuff.height);

        float cos_theta_i = refracted_lightRay_CC[2];//(-lensRay)*Vector3(0,0,-1);
        Vector3 result;
        spectrum_p_to_xyz(lensRay.lambda, radiance, result.data);
        accBuff.pixels[pixelPos] += result * cos_theta_i * sensitivity * 1000000;
        return 1;
    }
    return 0;
}


void ThinLensCamera::focus(float focalPlane) {
    sensorZPos = (focalDist * focalPlane) / (focalPlane - focalDist);
}


void ThinLensCamera::stopUp() {
    ap.stopUp();
    updateSampler();
}


void ThinLensCamera::stopDown() {
    ap.stopDown();
    updateSampler();
}


void ThinLensCamera::setStop(const float apertureRadius) {
    ap.scaleAbs(apertureRadius);
    updateSampler();
}


void ThinLensCamera::stats(char *text1, char *text2, char *text3, char *text4, char *text5, const int length) {
    snprintf(text1, 64, "focal length: %.1f mm", focalDist);
    snprintf(text2, 64, "sensor shift: %.4f mm", sensorZPos);
    snprintf(text3, 64, "aperture radius: %.2f mm", ap.radius);
    snprintf(text4, 64, "Sensitivity: %.2f mm", sensitivity);
}


void ThinLensCamera::zoomRel(float mult) {
    focalDist *= mult;
    sensorZPos = focalDist + 0.1f; //for easier view finding after zoom....
}


void ThinLensCamera::zoomAbs(float fl) {
    focalDist = fl;
    sensorZPos = focalDist + 0.1f; //for easier view finding after zoom....
}


void * ThinLensCamera::operator new(size_t bytes) {
    return _mm_malloc(sizeof(ThinLensCamera), 16);
}


