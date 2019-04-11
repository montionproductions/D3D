#pragma once
class Vector3D
{
 
public:
  Vector3D();
  Vector3D(float x_, 
           float y_, 
           float z_);
  ~Vector3D();

  bool 
  operator==(const Vector3D& v) const;

  Vector3D&
  operator+=(const Vector3D& v);

  Vector3D
  operator+(const Vector3D& v) const;

  Vector3D&
  operator-=(const Vector3D& v);

  Vector3D
  operator-(const Vector3D& v) const;

  Vector3D&
  operator*=(float scalar);

  Vector3D
  operator*(float scalar);

  union
  {
    struct
    {
      float x, y, z;
    };
    float data[3];
  };
};