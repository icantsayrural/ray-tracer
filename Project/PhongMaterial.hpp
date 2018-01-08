#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <lodepng/lodepng.h>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflection, double m_transparency, double m_refraction_index, const std::string &texture_fname, const std::string &bump_name);
  virtual ~PhongMaterial();

  int set_image_map(const std::string &file_name, std::vector<unsigned char> *map, int *width, int *height);
	glm::vec3 get_pixel(int x, int y, std::vector<unsigned char> *map, int width);
  glm::vec3 get_pixel_from_texture(int x, int y);
  glm::vec3 get_pixel_from_bump(int x, int y);

  glm::vec3 m_kd;
  glm::vec3 m_ks;

  bool m_has_texture;
  std::vector<unsigned char> m_texture;
  int m_texture_width;
  int m_texture_height;

	bool m_has_bump;
	std::vector<unsigned char> m_bump;
	int m_bump_width;
	int m_bump_height;

  double m_shininess;
  double m_reflection;
  double m_transparency;
  double m_refraction_index;
};
