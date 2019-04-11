#include "Vector3D.h"

#include <math.h>

Vector3D::Vector3D() {

}

Vector3D::Vector3D(float x_,
                   float y_,
                   float z_) {
  x = x_;
  y = y_;
  z = z_;
}

Vector3D::~Vector3D() {

}

bool
Vector3D::operator==(const Vector3D& v) const {
  return (x == v.x) && (y == v.y) && (z == v.z);
}

Vector3D&
Vector3D::operator+=(const Vector3D& v) {
  x += v.x;
  y += v.y;
  z += v.z;

  return *this;
}

Vector3D
Vector3D::operator+(const Vector3D& v) const {
  return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D&
Vector3D::operator-=(const Vector3D& v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;

  return *this;
}

Vector3D
Vector3D::operator-(const Vector3D& v) const {
  return Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D&
Vector3D::operator*=(float scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;

  return *this;
}

Vector3D
Vector3D::operator*(float scalar) {
  return Vector3D(x*scalar, y*scalar, z*scalar);
}

Vector3D&
Vector3D::operator/=(float scalar) {
  x /= scalar;
  y /= scalar;
  z /= scalar;

   return *this;
}

Vector3D
Vector3D::operator/(float scalar) {
  return Vector3D(x * scalar, y / scalar, z / scalar);
}

float
Vector3D::length() {
  return sqrtf(x*x + y*y + z*z);
}

float
Vector3D::dot(const Vector3D& v) {
  return x*v.x + y*v.y + z*v.z;
}

Vector3D
Vector3D::cross(const Vector3D& v) {
//uy vz - uz vy , uz vx - ux vz , ux vy - uy vx
  return Vector3D(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

Vector3D
Vector3D::normalize() {
  // d = u / ||u||
  Vector3D d;
  d = *this / this->length();
  return d;
}