#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include <cmath>

class Vector3 {
public:

    float data[4]; // x, y, z, w padding to 16 Byte...

    inline Vector3() {
        data[0] = data[1] = data[2] = 0.0;
    }


    inline Vector3(float x, float y, float z) {
        data[0] = x;
        data[1] = y;
        data[2] = z;
    }


    inline Vector3(const float &theta, const float &phi) {
        // dont use for rendering!!!
        data[0] = sinf(theta) * cosf(phi);
        data[1] = sinf(theta) * sinf(phi);
        data[2] = cosf(theta);
    }


    inline Vector3(const float *const f) {
        data[0] = f[0];
        data[1] = f[1];
        data[2] = f[2];
    }


    // element access lvalue
    inline float &operator[](unsigned char component) {
        return data[component];
    }


    // rvalue
    inline float operator[](unsigned char component) const {
        return data[component];
    }


    // operators
    inline Vector3 operator-() const { // unary -
        return Vector3(-data[0], -data[1], -data[2]);
    }


    inline void operator+=(const Vector3 &rhs) {
        data[0] += rhs.data[0];
        data[1] += rhs.data[1];
        data[2] += rhs.data[2];
    }


    inline void operator-=(const Vector3 &rhs) {
        data[0] -= rhs.data[0];
        data[1] -= rhs.data[1];
        data[2] -= rhs.data[2];
    }


    inline void operator*=(float alpha) {
        data[0] *= alpha;
        data[1] *= alpha;
        data[2] *= alpha;
    }


    inline void operator^=(const Vector3 &rhs) {
        data[0] *= rhs.data[0];
        data[1] *= rhs.data[1];
        data[2] *= rhs.data[2];
    }


    inline bool operator==(const Vector3 &v) {
        return v.data[0] == data[0] && v.data[1] == data[1] && v.data[2] == data[2];
    }


    inline void operator=(const Vector3 &rhs) {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        data[2] = rhs.data[2];
    }


    inline bool operator!=(const Vector3 &v) {
        return v.data[0] != data[0] || v.data[1] != data[1] || v.data[2] != data[2];
    }


    inline bool operator<<(const Vector3 &v) const {
        return data[0] < v.data[0] && data[1] < v.data[1] && data[2] < v.data[2];
    }


    inline bool operator<<(const float &f) const {
        return data[0] < f && data[1] < f && data[2] < f;
    }


    inline bool operator<(const Vector3 &v) const {
        return data[0] < v.data[0] || data[1] < v.data[1] || data[2] < v.data[2];
    }


    inline bool operator<(const float &f) const {
        return data[0] < f || data[1] < f || data[2] < f;
    }


    inline bool operator>(const Vector3 &v) const {
        return data[0] > v.data[0] || data[1] > v.data[1] || data[2] > v.data[2];

    }


    inline bool operator>(const float &f) const {
        return data[0] > f || data[1] > f || data[2] > f;

    }


    inline bool operator>>(const Vector3 &v) const {
        return data[0] > v.data[0] && data[1] > v.data[1] && data[2] > v.data[2];
    }


    inline bool operator>>(const float &f) const {
        return data[0] > f && data[1] > f && data[2] > f;

    }


    friend inline Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs) {
        return Vector3(lhs.data[0] + rhs.data[0],
                lhs.data[1] + rhs.data[1],
                lhs.data[2] + rhs.data[2]);
    }


    friend inline Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs) {
        return Vector3(lhs.data[0] - rhs.data[0],
                lhs.data[1] - rhs.data[1],
                lhs.data[2] - rhs.data[2]);
    }


    friend inline Vector3 operator+(const Vector3 &lhs, const float &rhs) {
        return Vector3(lhs.data[0] + rhs, lhs.data[1] + rhs, lhs.data[2] + rhs);
    }


    friend inline Vector3 operator-(const Vector3 &lhs, const float &rhs) {
        return Vector3(lhs.data[0] - rhs, lhs.data[1] - rhs, lhs.data[2] - rhs);
    }


    friend inline float operator*(const Vector3 &lhs, const Vector3 &rhs) { // dot product (Vector3 * Vector3)
        return (lhs.data[0] * rhs.data[0] + lhs.data[1] * rhs.data[1] + lhs.data[2] * rhs.data[2]);
    }


    friend inline Vector3 operator^(const Vector3 &lhs, const Vector3 &rhs) { // component-wise product (Vector3 * Vector3)
        return Vector3(lhs.data[0] * rhs.data[0],
                lhs.data[1] * rhs.data[1],
                lhs.data[2] * rhs.data[2]);
    }


    friend inline Vector3 operator%(const Vector3 &lhs, const Vector3 &rhs) { // cross product (Vector3 x Vector3)
        return Vector3(lhs.data[1] * rhs.data[2] - lhs.data[2] * rhs.data[1],
                lhs.data[2] * rhs.data[0] - lhs.data[0] * rhs.data[2],
                lhs.data[0] * rhs.data[1] - lhs.data[1] * rhs.data[0]);
    }


    friend inline Vector3 operator*(float alpha, const Vector3 &v) { // scalar multiplication (float * Vector3)
        return Vector3(alpha * v.data[0],
                alpha * v.data[1],
                alpha * v.data[2]);
    }


    friend inline Vector3 operator*(const Vector3 &v, float alpha) { // scalar multiplication (Vector3 * float)
        return Vector3(alpha * v.data[0],
                alpha * v.data[1],
                alpha * v.data[2]);
    }


    friend inline Vector3 operator/(const Vector3 &lhs, const Vector3 &rhs) { // (Vector3 / Vector3)
        return Vector3(lhs.data[0] / rhs.data[0],
                lhs.data[1] / rhs.data[1],
                lhs.data[2] / rhs.data[2]);
    }


    friend inline Vector3 operator/(const Vector3 &v, float alpha) { // (Vector3 / alpha)
        return Vector3(v.data[0] / alpha,
                v.data[1] / alpha,
                v.data[2] / alpha);
    }


    friend inline double HPrecDot(const Vector3 &lhs, const Vector3 &rhs) {

        return (double) lhs.data[0] * (double) rhs.data[0] +
                (double) lhs.data[1] * (double) rhs.data[1] +
                (double) lhs.data[2] * (double) rhs.data[2];
    }


    // length of vector
    inline float length() const {
        return sqrtf(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
    }


    // squared length of vector
    inline float lengthSquared() const {
        return (data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
    }


    // normalized copy of vector
    inline Vector3 normalizedCopy() const {
        const float recLen = 1.f / length();
        return Vector3(data[0] * recLen, data[1] * recLen, data[2] * recLen);
    }


    // normalized copy of vector and length
    inline Vector3 normalizedCopy(float &veclen) const {
        const float len = length();
        const float recLen = 1.f / len;
        return Vector3(data[0] * recLen, data[1] * recLen, data[2] * recLen);
    }


    inline float normalize() {
        const float len = length();
        const float recLen = 1.f / len;
        data[0] *= recLen;
        data[1] *= recLen;
        data[2] *= recLen;
        return len;
    }


    inline int getMaxIndex() const {
        if (data[0] > data[1]) return (data[2] > data[0]) ? 2 : 0;
        else return (data[2] > data[1]) ? 2 : 1;
    }


    inline float getMax() const {
        if (data[0] > data[1]) {
            return (data[2] > data[0]) ? data[2] : data[0];
        }
        else {
            return (data[2] > data[1]) ? data[2] : data[1];
        }
    }


    inline int getMinIndex() const {
        if (data[0] < data[1]) {
            return (data[2] < data[0]) ? 2 : 0;
        }
        else {
            return (data[2] < data[1]) ? 2 : 1;
        }
    }


    inline int getMinIndexAbs() const {
        if (fabs(data[0]) < fabs(data[1])) return (fabs(data[2]) < fabs(data[0])) ? 2 : 0;
        else return (fabs(data[2]) < fabs(data[1])) ? 2 : 1;
    }


    inline void clamp() {
        for (int i = 0; i < 3; i++) {
            if (data[i] < 0.0f) data[i] = 0.0f;
            else if (data[i] > 1.0f) data[i] = 1.0f;
        }
    }


    inline void clampNegative() {

        data[0] = data[0] > 0.0f ? data[0] : 0.0f;
        data[1] = data[1] > 0.0f ? data[1] : 0.0f;
        data[2] = data[2] > 0.0f ? data[2] : 0.0f;
    }


    inline friend Vector3 &vabs(Vector3 &v) {
        v.data[0] = fabsf(v.data[0]);
        v.data[1] = fabsf(v.data[1]);
        v.data[2] = fabsf(v.data[2]);
        return v;
    }
};


inline void getCoordSystem(const Vector3 &n, Vector3 &t1, Vector3 &t2) {
    t1 = n;
    t1[t1.getMinIndexAbs()] = 1.0f;
    t1 = t1 % n;
    t1.normalize();
    t2 = n % t1;
    t2.normalize();
}


inline Vector3 minVector(const Vector3 &v1, const Vector3 &v2) {
    Vector3 result;
    result[0] = fminf(v1[0], v2[0]);
    result[1] = fminf(v1[1], v2[1]);
    result[2] = fminf(v1[2], v2[2]);
    return result;
}


inline Vector3 maxVector(const Vector3 &v1, const Vector3 &v2) {

    Vector3 result;

    result[0] = fmaxf(v1[0], v2[0]);
    result[1] = fmaxf(v1[1], v2[1]);
    result[2] = fmaxf(v1[2], v2[2]);

    return result;
}


inline Vector3 mirror(const Vector3 &dir, const Vector3 &mirror_Vec) {
    //input:
    // dir must show in same direction as normal....
    return (2.0f * ((dir) * mirror_Vec)) * mirror_Vec - dir; // mirror direction
}
//
//inline bool equals(const Vector3 &v1, const Vector3 &v2) {
//
//    bool bx = equals(v1[0], v2[0]);
//    bool by = equals(v1[1], v2[1]);
//    bool bz = equals(v1[2], v2[2]);
//
//    return bx && by && bz;
//}


#endif


