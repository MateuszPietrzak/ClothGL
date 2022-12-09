//
// Created by drabart on 09.12.22.
//

#include "../../include/Vector3.h"

Vector3::Vector3(float _x, float _y, float _z) :
    x(_x),
    y(_y),
    z(_z){

}

Vector3 Vector3::operator+(const Vector3 &other) const {
    return Vector3(this->x + other.x, this->y + other.y, this->z + other.z);
}

Vector3 Vector3::operator+=(const Vector3 &other) {
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
}

Vector3 Vector3::operator-(const Vector3 &other) const {
    return Vector3(this->x - other.x, this->y - other.y, this->z - other.z);
}

Vector3 Vector3::operator-=(const Vector3 &other) {
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
}

Vector3 Vector3::operator*(const float &other) const {
    return Vector3(this->x * other, this->y * other, this->z * other);
}

Vector3 Vector3::operator*=(const float &other) {
    this->x *= other;
    this->y *= other;
    this->z *= other;
}

Vector3 Vector3::operator/(const float &other) const {
    return Vector3(this->x / other, this->y / other, this->z / other);
}

Vector3 Vector3::operator/=(const float &other) {
    this->x /= other;
    this->y /= other;
    this->z /= other;
}

inline float sqrt(float x)
{
    float px = x;
    float xhalf = 0.5f * x;
    int i = *(int*)&x;            // store floating-point bits in integer
    i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
    x = *(float*)&i;              // convert new bits into float
    x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
    return x * px;
}

float Vector3::length2() {
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

float Vector3::length() {
    return sqrt(this->length2());
}

float Vector3::normalize() {
    float len = this->length();
    this->x /= len;
    this->y /= len;
    this->z /= len;
}

float Vector3::dist(const Vector3 &other) {
    Vector3 v = *this - other;
    return v.length();
}

float Vector3::dot_product(const Vector3& other) {
    return this->x * other.x + this->y * other.y + this->z * other.z;
}
