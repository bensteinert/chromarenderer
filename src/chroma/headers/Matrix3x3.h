#ifndef MATRIX_H
#define MATRIX_H

#include "Vector3.h"

using namespace std;

class Matrix3x3
{
public:

	static Matrix3x3 ZEROMATRIX;
	static Matrix3x3 ONEMATRIX;
	static Matrix3x3 IDENTITY;
	static Matrix3x3 RGBTOXYZ;
	static Matrix3x3 XYZTORGB;


	Matrix3x3();
	Matrix3x3(const Matrix3x3& in);
	Matrix3x3(const Vector3& v1, const Vector3& v2, const Vector3& v3,bool transposedVector3s);
	Matrix3x3(const Vector3& v1, const Vector3& v2, const Vector3& v3);
	void debug();
	void invert();
	void orthogonalize();
	void toArray16(float* arr);
	void invertToArray16(float* arr);
	static inline Matrix3x3 getCrossproductMatrix(const Vector3& v) { return Matrix3x3(Vector3(0, v[2], -v[1]), Vector3(-v[2], 0, v[0]), Vector3(v[1], -v[0], 0)); }
	Matrix3x3 transpose();

	Vector3 operator *(const Vector3& v) const;
	Vector3& operator [](const int& colnum);
	const Vector3& operator [](const int& colnum) const;

	Matrix3x3 operator -();
	Matrix3x3 operator *(const Matrix3x3& m) const;
	Matrix3x3 operator *(const float& m) const;
	Matrix3x3 operator +(const Matrix3x3& m) const;
	void operator +=(const Matrix3x3& m);

	void operator /=(const float& div);
	void operator *=(const float& factor);

	friend std::ostream &operator<<(std::ostream &os, const Matrix3x3 &m)
	{
	    os << m.col1[0] << ", " << m.col2[0] <<  ", " << m.col3[0] << endl;
	    os << m.col1[1] << ", " << m.col2[1] <<  ", " << m.col3[1] << endl;
	    os << m.col1[2] << ", " << m.col2[2] <<  ", " << m.col3[2] << endl;
	    return os;
	}

	Vector3 col1;
	Vector3 col2;
	Vector3 col3;
};

#endif
