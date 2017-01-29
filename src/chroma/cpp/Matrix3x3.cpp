#include <stdio.h>

#include "Matrix3x3.h"

Matrix3x3 Matrix3x3::RGBTOXYZ = Matrix3x3(Vector3(0.5149, 0.2654, 0.0248), Vector3(0.3244, 0.6704, 0.1248),
                                          Vector3(0.1607, 0.0642, 0.8504));
Matrix3x3 Matrix3x3::XYZTORGB = Matrix3x3(Vector3(2.5623, -1.0215, 0.0752), Vector3(-1.1661, 1.9778, -0.2562),
                                          Vector3(-0.3962, 0.0437, 1.1810));
Matrix3x3 Matrix3x3::IDENTITY = Matrix3x3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
Matrix3x3 Matrix3x3::ZEROMATRIX = Matrix3x3(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
Matrix3x3 Matrix3x3::ONEMATRIX = Matrix3x3(Vector3(1, 1, 1), Vector3(1, 1, 1), Vector3(1, 1, 1));


Matrix3x3::Matrix3x3() : col1(0, 0, 0), col2(0, 0, 0), col3(0, 0, 0) { }


Matrix3x3::Matrix3x3(const Matrix3x3 &in) : col1(in.col1), col2(in.col2), col3(in.col3) {
}


Matrix3x3::Matrix3x3(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) : col1(v1), col2(v2), col3(v3) {
}


Matrix3x3::Matrix3x3(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, bool transposedVector3s) {
    if (transposedVector3s) {
        col1 = Vector3(v1[0], v2[0], v3[0]);
        col2 = Vector3(v1[1], v2[1], v3[1]);
        col3 = Vector3(v1[2], v2[2], v3[2]);

    }
    else
        Matrix3x3(v1, v2, v3);
}


void Matrix3x3::debug() {
//	for(int i = 0; i < 3; i++) //Zeilen
//	{
//		printf("\n");
//		for(int j = 0; j < 3; j++) // Spalten
//		{
//			printf(" %.4f ",( (&this[i])[j] ));
//		}
//	}
    printf("\n");

}


void Matrix3x3::invert() {
    float temp[9];
    float invDet = 1.f / (col1[0] * col2[1] * col3[2] - col1[0] * col3[1] * col2[2] - col2[0] * col1[1] * col3[2]
                          + col2[0] * col3[1] * col1[2] + col3[0] * col1[1] * col2[2] - col3[0] * col2[1] * col1[2]);

    temp[0] = col2[1] * col3[2] - col3[1] * col2[2];
    temp[1] = col3[0] * col2[2] - col2[0] * col3[2];
    temp[2] = col3[0] * col3[1] - col3[0] * col2[1];
    temp[3] = col3[1] * col1[2] - col3[2] * col1[1];
    temp[4] = col1[0] * col3[2] - col3[0] * col1[2];
    temp[5] = col2[0] * col1[2] - col1[0] * col2[2];
    temp[6] = col1[1] * col2[2] - col1[2] * col2[1];
    temp[7] = col3[0] * col1[1] - col1[0] * col3[1];
    temp[8] = col1[0] * col2[1] - col2[0] * col1[1];

    for (int i = 0; i < 3; i++) {
        this->col1[i] = invDet * temp[i * 3];
        this->col2[i] = invDet * temp[i * 3 + 1];
        this->col3[i] = invDet * temp[i * 3 + 2];
    }
}


void Matrix3x3::orthogonalize() {
    Vector3 tempA2, tempA3;
    col1 = col1 * (1.0f / col1.length());
    tempA2 = col2 - (col1 * col2) * col1;
    col2 = tempA2 * (1.0f / tempA2.length());
    tempA3 = col3 - (col1 * col3) * col1 - (col2 * col3) * col2;
    col3 = tempA3 * (1.0f / tempA3.length());
}


Matrix3x3 Matrix3x3::transpose() {
    Matrix3x3 m;

    m.col1[0] = col1[0];
    m.col1[1] = col2[0];
    m.col1[2] = col3[0];

    m.col2[0] = col1[1];
    m.col2[1] = col2[1];
    m.col2[2] = col3[1];

    m.col3[0] = col1[2];
    m.col3[1] = col2[2];
    m.col3[2] = col3[2];

    return m;
}


Matrix3x3 Matrix3x3::operator+(const Matrix3x3 &m) const {
    return Matrix3x3(col1 + m.col1, col2 + m.col2, col3 + m.col3);
}


Matrix3x3 Matrix3x3::operator*(const float &m) const {
    return Matrix3x3(col1 * m, col2 * m, col3 * m);
}


void Matrix3x3::toArray16(float *arr) {
    arr[0] = col1[0];
    arr[1] = col1[1];
    arr[2] = col1[2];
    arr[3] = 0;
    arr[4] = col2[0];
    arr[5] = col2[1];
    arr[6] = col2[2];
    arr[7] = 0;
    arr[8] = col3[0];
    arr[9] = col3[1];
    arr[10] = col3[2];
    arr[11] = 0;
    arr[12] = 0;
    arr[13] = 0;
    arr[14] = 0;
    arr[15] = 1;

}


void Matrix3x3::invertToArray16(float *arr) {
    Matrix3x3 temp = Matrix3x3(col1, col2, col3);
    temp.invert();

    arr[0] = temp.col1[0];
    arr[1] = temp.col1[1];
    arr[2] = temp.col1[2];
    arr[3] = 0;
    arr[4] = temp.col2[0];
    arr[5] = temp.col2[1];
    arr[6] = temp.col2[2];
    arr[7] = 0;
    arr[8] = temp.col3[0];
    arr[9] = temp.col3[1];
    arr[10] = temp.col3[2];
    arr[11] = 0;
    arr[12] = 0;
    arr[13] = 0;
    arr[14] = 0;
    arr[15] = 1;

}


Vector3 Matrix3x3::operator*(const Vector3 &v) const {
    Vector3 line1(col1[0], col2[0], col3[0]);
    Vector3 line2(col1[1], col2[1], col3[1]);
    Vector3 line3(col1[2], col2[2], col3[2]);
    return Vector3(line1 * v, line2 * v, line3 * v);
}


Vector3 &Matrix3x3::operator[](const int &column) {
    //if (column == 0)
    //	return col1;
    //else if (column == 1)
    //	return col1;
    //else if (column == 2)
    //	return col2;

    return (&col1)[column];
}


const Vector3 &Matrix3x3::operator[](const int &column) const {
    return (&col1)[column];
}


Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &mIn) const {
    Vector3 line1(col1[0], col2[0], col3[0]);
    Vector3 line2(col1[1], col2[1], col3[1]);
    Vector3 line3(col1[2], col2[2], col3[2]);

    return Matrix3x3(Vector3(line1 * mIn.col1, line2 * mIn.col1, line3 * mIn.col1),
                     Vector3(line1 * mIn.col2, line2 * mIn.col2, line3 * mIn.col2),
                     Vector3(line1 * mIn.col3, line2 * mIn.col3, line3 * mIn.col3));
}


void Matrix3x3::operator+=(const Matrix3x3 &m) {
    col1 += m.col1;
    col2 += m.col2;
    col3 += m.col3;
}


void Matrix3x3::operator/=(const float &div) {
    float invDiv = 1.f / div;
    col1 *= invDiv;
    col2 *= invDiv;
    col3 *= invDiv;
}


void Matrix3x3::operator*=(const float &factor) {
    col1 *= factor;
    col2 *= factor;
    col3 *= factor;
}


Matrix3x3 Matrix3x3::operator-() {
    col1 = -col1;
    col2 = -col2;
    col3 = -col3;
    return *this;
}


