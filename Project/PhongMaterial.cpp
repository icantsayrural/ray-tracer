#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
  const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflection, double transparency, double refraction_index, const std::string &texture_fname, const std::string &bump_fname)
  : m_kd(kd)
  , m_ks(ks)
  , m_shininess(shininess)
  , m_reflection(reflection)
  , m_transparency(transparency)
  , m_refraction_index(refraction_index)
{
  m_has_texture = texture_fname.size() != 0 && set_image_map(texture_fname, &m_texture, &m_texture_width, &m_texture_height) == 0;
	m_has_bump = bump_fname.size() != 0 && set_image_map(bump_fname, &m_bump, &m_bump_width, &m_bump_height) == 0;
}

PhongMaterial::~PhongMaterial()
{}


int PhongMaterial::set_image_map(const std::string &file_name, std::vector<unsigned char> *map,
		int *width, int *height) {
  unsigned w = 0;
  unsigned h = 0;

  std::vector<unsigned char> tmp_image;
  unsigned tmp_w;
  unsigned tmp_h;

  unsigned err = lodepng::decode(tmp_image, tmp_w, tmp_h, file_name);

  if (err) {
    return err;
  }

  w += tmp_w;
  h += tmp_h;

  map->insert(map->end(), tmp_image.begin(), tmp_image.end());
  *width = w;
  *height = h;

  return 0;
}

glm::vec3 PhongMaterial::get_pixel(int x, int y, std::vector<unsigned char> *map, int width) {
  int index = 4 * (width * y + x);
  double r = (double) (*map)[index] / 225;
  double g = (double) (*map)[index + 1] / 225;
  double b = (double) (*map)[index + 2] / 255;

  return glm::vec3(r, g, b);
}

glm::vec3 PhongMaterial::get_pixel_from_texture(int x, int y) {
	return m_has_texture ? get_pixel(x, y, &m_texture, m_texture_width) : glm::vec3(0);
}

glm::vec3 PhongMaterial::get_pixel_from_bump(int x, int y) {
	return m_has_bump ? get_pixel(x, y, &m_bump, m_bump_width) : glm::vec3(0);
}
