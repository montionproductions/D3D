#include "Vector3D.h"

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