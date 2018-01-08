#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
  size_t v1;
  size_t v2;
  size_t v3;

  size_t vn1;
  size_t vn2;
  size_t vn3;

  Triangle(size_t pv1, size_t pv2, size_t pv3,
      size_t pvn1, size_t pvn2, size_t pvn3)
    : v1( pv1 )
    , v2( pv2 )
    , v3( pv3 )
    , vn1(pvn1)
    , vn2(pvn2)
    , vn3(pvn3)
  {}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );

  std::vector<glm::vec3> m_vertices;
  std::vector<Triangle> m_faces;
  std::vector<glm::vec3> m_vertex_normals;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
