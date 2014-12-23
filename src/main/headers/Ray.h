#ifndef RAY_H
#define RAY_H


class Ray {

public:
    Vector3 origin, direction, inv_direction;    //36
    Vector3 spectrum;                            //12
    float tmin, tmax; // interval				// 8
    float lambda;                            // 4
    char sign[3];                                // 3
    bool spectral;                                // 1

    Ray();
    Ray(const Vector3 &origin, const Vector3 &direction, const float tmin, const float tmax, const float lambda_in);
    Vector3 at(const float t);

};

#endif
