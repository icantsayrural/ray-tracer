#include "Primitive.hpp"

Primitive::Primitive()
{
  m_primType = PrimType::Prim;
}

Primitive::~Primitive()
{
}

Sphere::Sphere()
{
  m_primType = PrimType::Sphere;
}

Sphere::~Sphere()
{
}

Cube::Cube()
{
  m_primType = PrimType::Cube;
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}

Particles::Particles() {
  m_primType = PrimType::Particles;
}

Particles::~Particles() {
}
