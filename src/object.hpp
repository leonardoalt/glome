#pragma once

#include "matrix4.hpp"

class Object
{
public:
  Object(const Matrix4& init = Matrix4::IDENTITY):
    t(init)
  {}
  virtual ~Object() = 0;

  const Matrix4& transformation() const
  {
    return t;
  }

protected:
  Matrix4 t;
};
