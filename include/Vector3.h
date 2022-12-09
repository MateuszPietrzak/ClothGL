//
// Created by drabart on 09.12.22.
//

#ifndef CLOTHGL_VECTOR3_H
#define CLOTHGL_VECTOR3_H

class Vector3 {
public:
    Vector3();
    Vector3(float _x, float _y);
    Vector3(float _x, float _y, float _z);

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator+=(const Vector3& other);
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator-=(const Vector3& other);
    Vector3 operator*(const float& other) const;
    Vector3 operator*=(const float& other);
    Vector3 operator/(const float& other) const;
    Vector3 operator/=(const float& other);

    float length();
    float length2();
    void normalize();
    float dist(const Vector3& other);
    float dot_product(const Vector3& other);

    float x, y, z;
};

#endif //CLOTHGL_VECTOR3_H
