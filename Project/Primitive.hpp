#pragma once

#include <glm/glm.hpp>

enum class PrimType {
  Prim,
  Sphere,
  Mesh,
  Cube,
  SphereEasy,
  CubeEasy,
  Particles
};

class Primitive {
public:
  Primitive();
  virtual ~Primitive();
  PrimType m_primType;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
    m_primType = PrimType::SphereEasy;
  }
  virtual ~NonhierSphere();

  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
    m_primType = PrimType::CubeEasy;
  }
  
  virtual ~NonhierBox();

  glm::vec3 m_pos;
  double m_size;
};

class Particles : public Primitive {
  public:
    Particles();
    virtual ~Particles();
};
