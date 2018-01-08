#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
  : m_vertices()
  , m_faces()
  , m_vertex_normals()
{
  std::string code;
  double vx, vy, vz;

  std::string tmp;
  size_t v[3];
  size_t vn[3];
  std::ifstream ifs( fname.c_str() );
  while( ifs >> code ) {
    if( code == "v" ) {
      ifs >> vx >> vy >> vz;
      m_vertices.push_back( glm::vec3( vx, vy, vz ) );
    } else if( code == "f" ) {
      for (int i = 0; i < 3; i++) {
        ifs >> tmp;
        sscanf(tmp.c_str(), "%zd//%zd", &(v[i]), &(vn[i]));
      }
      m_faces.push_back( Triangle(v[0] - 1, v[1] - 1, v[2] - 1, vn[0] - 1, vn[1] - 1, vn[2] - 1) );
    } else if (code == "vn") {
      ifs >> vx >> vy >> vz;
      m_vertex_normals.push_back(glm::vec3(vx, vy, vz));
    }
  }
  m_primType = PrimType::Mesh;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  //out << glm::to_string(mesh.m_vertices[0]);
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
    const MeshVertex& v = mesh.m_verts[idx];
    out << glm::to_string( v.m_position );
  if( mesh.m_have_norm ) {
      out << " / " << glm::to_string( v.m_normal );
  }
  if( mesh.m_have_uv ) {
      out << " / " << glm::to_string( v.m_uv );
  }
  }

*/
  out << "}";
  return out;
}
