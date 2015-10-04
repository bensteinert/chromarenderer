#include "Camera.h"
#include "Material.h"
#include "Chroma.h"


Camera::Camera(const string &filename, const int xres, const int yres, const float sensitivity_in) :
        pos(Vector3()), rot(Matrix3x3::IDENTITY), inv_rot(Matrix3x3::IDENTITY), save_rot(Matrix3x3::IDENTITY),
        save_pos(Vector3()),
        lensSampler(0), neeLens(0), sensitivity(sensitivity_in), sensorZPos(0.0f), focalDist(53.0f),
        xRes(xres), yRes(yres), sensorSizeX(FILMSIZEX), sensorSizeY(FILMSIZEX / ((float) xres / yres)),
        sensorArea(sensorSizeX * sensorSizeY),
        pixelSizeX(sensorSizeX / xres), pixelSizeY(sensorSizeY / yres),
        shift(Vector3(sensorSizeX * 0.5f, sensorSizeY * 0.5f, 0)) {
    LoadCamera(filename);
}


Camera::~Camera() {
    delete lensSampler;
}


void Camera::reset() {
    pos = save_pos;
    rot = save_rot;
    inv_rot = rot.transpose(); // rot othonormal so it is ok ;)
}


int Camera::LoadCamera(const string &filename) {
    string path = string(scene_path);
    path.append(filename);
    path.append("/");
    path.append(filename);

    /*Camera*/
    path.append(".cam");

    FILE *myfile = fopen(path.c_str(), "rb");
    float *buffer = 0;

    if (!myfile) {
        cout << "file " << path << " not found. CAM loader skipped." << endl;
    }
    else {

        fseek(myfile, 0, SEEK_SET);
        float *buffer = new float[11];

        if (fread(buffer, sizeof(float), 11, myfile) != 11) {
            cout << "Corrupted CAM file " << path << "! Aborting" << endl;
            fclose(myfile);
            delete[] buffer;
            return 1;
        }

        cout << "Loading Camera from " << path << "... ";
        loadCameraSettings(buffer);
        cout << " finished" << endl;
        fclose(myfile);
    }

    delete[] buffer;
    return 0;
}


void Camera::loadCameraSettings(const float *const vals) {

    pos = Vector3(vals);
    rot[2] = Vector3(vals + 3).normalizedCopy();
    rot[1] = Vector3(vals + 6).normalizedCopy();
    rot[0] = (rot[1] % rot[2]).normalizedCopy();
    if (!equals(sensorSizeX / sensorSizeY, 4.0f / 3.0f))
        cout << "non 4:3 sensor format!" << endl;
    float crop = 32.0f / sensorSizeX;                    //assume 4:3 format as set!
    focalDist = vals[9] * 1000.0f /
                crop;                //vals[9] is focal Distance at 32x24mm 'film' so introduce crop factor
    save_rot = rot;
    inv_rot = rot.transpose();
    save_pos = pos;
}


void Camera::rotate(const int &deg_x, const int &deg_y) {
    float sin_t = sin(RAD(deg_y));
    float cos_t = cos(RAD(deg_y));
    float sin_p = sin(RAD(deg_x));
    float cos_p = cos(RAD(deg_x));

    rot = rot * Matrix3x3(Vector3(cos_p, sin_t * sin_p, -sin_p * cos_t), Vector3(0, cos_t, sin_t),
                          Vector3(sin_p, (-1.0) * sin_t * cos_p, cos_t * cos_p));;
    rot[0].normalize();
    rot[1].normalize();
    rot[2].normalize();
    inv_rot = rot.transpose(); // rot othonormal so it is ok ;)
}


void Camera::move(const Vector3 &move) {

    pos = pos + rot * move;
}


void Camera::superSample(const float scaling) {

    assert(equals(sensorSizeX / sensorSizeY, ASPECTRATIO));    //scaling assumes 4:3 sensor

    sensorSizeX *= scaling;
    sensorSizeY *= scaling;
    pixelSizeX *= scaling;
    pixelSizeY *= scaling;
    sensorArea = sensorSizeX * sensorSizeY;

    shift = Vector3(sensorSizeX * 0.5f, sensorSizeY * 0.5f, 0);
}


int Camera::sensorHit(const Vector3 &sensorPlanePoint, const int resX, const int resY) {

    Vector3 filmHit = sensorPlanePoint + shift; // shift to avoid negative values in pixel finding

    if (filmHit[0] > 0.0f && filmHit[0] < sensorSizeX &&
        filmHit[1] > 0.0f && filmHit[1] < sensorSizeY) {

        int pixelPos = ((int) (filmHit[1] / pixelSizeY)) * (resX) +
                       ((resX - 1) - (int) (filmHit[0] / pixelSizeX)); // pixel position on the sensor
        DBG_ASSERT(pixelPos < resX * resY);
        return pixelPos;
    }
    return -1;
}


Vector3 Camera::pointInCamCoords(const Vector3 &point) {
    return (inv_rot * (point - pos)) * 1000.0f;
}


Vector3 Camera::pointInWorldCoords(const Vector3 &point) {
    return (rot * (point * 0.001f)) + pos;
}


Vector3 Camera::dirInCamCoords(const Vector3 &dir) {
    return inv_rot * dir;
}


Vector3 Camera::dirInWorldCoords(const Vector3 &dir) {
    return rot * dir;
}


Ray Camera::rayInWorldCoords(const Vector3 &origin, const Vector3 &dir, const float lambda) {
    return Ray((rot * (origin * 0.001f)) + pos, rot * dir, EPS, FLT_MAX, lambda);
}


Ray Camera::rayInCamCoords(const Vector3 &origin, const Vector3 &dir, const float lambda) {
    return Ray((inv_rot * (origin - pos)) * 1000.0f, inv_rot * dir, EPS, FLT_MAX, lambda);
}


void Camera::toCamCoordSys(Ray &ray) {
    ray = Ray((inv_rot * (ray.origin - pos)) * 1000.0f, inv_rot * ray.direction, ray.tMin, ray.tMax, ray.lambda);
}


void Camera::toWorldCoordSys(Ray &ray) {
    ray = Ray((rot * (ray.origin * 0.001f)) + pos, rot * ray.direction, ray.tMin, ray.tMax, ray.lambda);
}


void Camera::zShift(float z) {
    sensorZPos += z;
}


Vector3 Camera::getPixelCoord(const float x, const float y, const float shift_x, const float shift_y) {
    return Vector3(-sensorSizeX * 0.5f + pixelSizeX * (x + shift_x),
                   -sensorSizeY * 0.5f + pixelSizeY * (y + shift_y), sensorZPos);
    //return Vector3(0.5f*pixelSizeX*(x+shift_x), 0.5f*pixelSizeY*(y+shift_y), sensorZPos);
    //return Vector3(0.5f*pixelSizeX*(x+shift_x) - 64*pixelSizeX, 0.5f*pixelSizeY*(y+shift_y)-64*pixelSizeY, sensorZPos);
}


std::ostream &operator<<(std::ostream &os, const Camera &camera) {
    //os << "Camera Model:\t\tThin Lens" << endl;
    os << "Position\t\t" << camera.pos << endl;
    os << "Rotation Matrix:\t" << camera.rot.col1[0] << ", " << camera.rot.col2[0] <<  ", " << camera.rot.col3[0] << endl;
    os << "\t\t\t" <<       	camera.rot.col1[1] << ", " << camera.rot.col2[1] <<  ", " << camera.rot.col3[1] << endl;
    os << "\t\t\t" << 			camera.rot.col1[2] << ", " << camera.rot.col2[2] <<  ", " << camera.rot.col3[2] << endl;
    os << "Resolution:\t\t" << camera.xRes << "  x  " << camera.yRes << endl;
    os << "Sensor dimensions:\t" << camera.sensorSizeX << "mm  x  " << camera.sensorSizeY << "mm" << endl;
    os << "EFL:\t\t\t" << camera.focalDist << "mm" << endl;
    os << "Lens-Sensor distance:\t" << camera.sensorZPos << "mm" << endl;
    os << "Sensitivity:\t\t" << camera.sensitivity << endl;
    os << "F-number:\t\t" << camera.stopNumber;
    return os;
}


void Camera::lensContribution(bool contributes) {
    lensSampler->insideMat->contribution = contributes;
}
