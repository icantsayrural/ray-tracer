#include <glm/ext.hpp>
#include <limits>
#include <vector>

#include "Project.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Particle.hpp"

// Constant values
const double PI = 3.14;
const double SUBPIXEL = 3;
const double RECURSE = 3;
const double AIR_INDEX = 1.52;
const int NUM_SHADOW_RAYS = 10;
const double SHADOW_SHIFT = 10;

// Booleans for turning features on and off
const bool BOUNDING = true;
const bool SUPER_SAMPLING = true;
const bool PHONG = true;
const bool CSG = true;

// For cube intersections, it's easier to statically store
// the vertices, faces and face normals
const int cube_faces_array[] = {
  1,2,3,8,
  3,4,6,8,
  7,5,4,6,
  2,5,7,1,
  6,7,1,8,
  2,3,4,5
};

const glm::vec3 cube_verts_array[] = {
  glm::vec3(1,1,0),
  glm::vec3(1,0,0),
  glm::vec3(1,0,1),
  glm::vec3(0,0,1),
  glm::vec3(0,0,0),
  glm::vec3(0,1,1),
  glm::vec3(0,1,0),
  glm::vec3(1,1,1)
};

const glm::vec3 cube_normals[] = {
  glm::vec3(1,0,0),
  glm::vec3(0,0,1),
  glm::vec3(-1,0,0),
  glm::vec3(0,0,-1),
  glm::vec3(0,1,0),
  glm::vec3(0,-1,0)
};

void Project_Render(
    // What to render
    SceneNode * root,

    // Image to write to, set to a given width and height
    Image & image,

    // Viewing parameters
    const glm::vec3 & eye, // location of eye
    const glm::vec3 & view, // direction of eyesight
    const glm::vec3 & up,
    double fovy,

    // Lighting parameters
    const glm::vec3 & ambient,
    const std::list<Light *> & lights,

    // Particle system
    ParticleSystem *ps
) {
  // Compute position of image
  size_t h = image.height();
  size_t w = image.width();

  // Particle system init
  ps->set_dims((double) w, (double) h);
  ps->add_particles();

  // Compute view related things
  glm::vec3 view_vector = glm::normalize(view);
  glm::vec3 U = glm::normalize(glm::cross(view_vector, up));
  glm::vec3 V = glm::normalize(glm::cross(U, view_vector));

  double half_height = tan(fovy/2 * 180/PI);
  double half_width = (w/h) * half_height;
  glm::vec3 bottom_left_point = (eye + view_vector) - (V * half_height) - (U * half_width);

  glm::vec3 x_incr = (2 * U * half_width) / w;
  glm::vec3 y_incr = (2 * V * half_height) / h;

  // Loop over pixels
  int nodes[w][h];
  int faces[w][h];

  for (uint y = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x) {
      image(x, h-y-1, 0) = 0;
      image(x, h-y-1, 1) = 0;
      image(x, h-y-1, 2) = 0;

      bool perform_supersampling = SUPER_SAMPLING;
      double end = SUBPIXEL;

      // Determine the nodeId of the nearest node intersection at this pixel
      // We will use previously obtained nodeId information for adaptive antialising
      // If a pixel is adjacent to pixels with the same nodeId, we shouldn't do supersampling
      glm::vec3 pixel_eye = bottom_left_point + ((x + 0.5) * x_incr) + ((y + 0.5) * y_incr);
      glm::vec3 vector = glm::normalize(pixel_eye - eye);

      glm::vec3 middle_intersection;
      glm::vec3 middle_local_intersection;
      glm::vec3 middle_normal;
      int faceId;

      GeometryNode *nearest_middle_node = find_node_with_smallest_t(eye, vector, root, &middle_intersection, &middle_local_intersection, &middle_normal, &faceId, NULL, ps);
      int nearest_nodeId = nearest_middle_node == NULL ? -1 : nearest_middle_node->m_nodeId;
      nodes[x][y] = nearest_nodeId;
      faces[x][y] = faceId;

      if ((x == 0 || (nodes[x-1][y] == nearest_nodeId && faces[x-1][y] == faceId)) &&
          (y == 0 || (nodes[x][y-1] == nearest_nodeId && faces[x][y-1] == faceId)) &&
          (x == 0 || y == 0 || (nodes[x-1][y-1] == nearest_nodeId && faces[x-1][y-1] == faceId))) {
        end = 1;
        perform_supersampling = false;
      }

      // Lighting calculation work begins here
      for (int i = 0; i < SUBPIXEL; ++i) {
        if (!perform_supersampling && i != 1) {
          continue;
        }
        for (int j = 0; j < SUBPIXEL; ++j) {
          if (!perform_supersampling && j != 1) {
            continue;
          }

          double ys = i / (SUBPIXEL - 1) + y;
          double xs = j / (SUBPIXEL - 1) + x;

          pixel_eye = bottom_left_point + (xs * x_incr) + (ys * y_incr);
          vector = glm::normalize(pixel_eye - eye);

          // Primary ray
          glm::vec3 intersection = (i == 1 && j == 1) ? middle_intersection : glm::vec3(0);
          glm::vec3 local_intersection = (i == 1 && j == 1) ? middle_local_intersection : glm::vec3(0);
          glm::vec3 normal = (i == 1 && j == 1) ? middle_normal : glm::vec3(0);
          GeometryNode *nearest_node = (i == 1 && j == 1) ? nearest_middle_node : find_node_with_smallest_t(eye, vector, root, &intersection, &local_intersection, &normal, &faceId, NULL, ps);
          glm::vec3 colour(0, 0, 0);
          glm::vec3 texture(0, 0, 0);

          if (nearest_node != NULL) {
            // There is nearest node, let's cast shadow ray from the intersection to all the lights
            for (Light *light : lights) {
              // Shadow ray logic:
              // We want to distribute shadow rays over an area of the area source
              // To acheive "soft" shadows
              bool shadow = true;
              glm::vec3 radiance(0, 0, 0);

              double num_shadow_rays = 1 + NUM_SHADOW_RAYS * 2;
              for (double s = -NUM_SHADOW_RAYS; s <= NUM_SHADOW_RAYS; s++) {
                glm::vec3 some_normal;
                glm::vec3 some_intersection;
                glm::vec3 some_local_intersection;

                glm::vec3 modified_light_position = light->position + glm::vec3(0, 0, s * SHADOW_SHIFT);
                glm::vec3 modified_light_vector = glm::normalize(modified_light_position - intersection);

                GeometryNode *obscuring_node = find_node_with_smallest_t(intersection, modified_light_vector, root, &some_intersection, &some_local_intersection, &some_normal, &faceId, nearest_node, ps);

                if (obscuring_node == NULL) {
                  // Compute radiance
                  // The radiance contributed by this shadow ray is E/M * dot(normal, modified_light_vector)
                  double distance_to_light = glm::distance(modified_light_position, intersection);
                  glm::vec3 partial_radiance = compute_incoming_radiance(light, distance_to_light) / num_shadow_rays;
                  radiance += partial_radiance;
                  shadow = false;
                }
              }
              // If there is an obscuring node that gets in the way between the light source
              // and our surface, no light can contribute (besides ambient lighting)
              if (shadow) {
                continue;
              }

              // Actual light vector
              glm::vec3 light_vector = glm::normalize(light->position - intersection);

              // Compute reflection and refraction
              glm::vec3 reflection_colour = generate_reflection_rays(nearest_node, intersection, vector, normal, root, RECURSE, ps);
              glm::vec3 refraction_colour = generate_refraction_rays(nearest_node, intersection, vector, normal, root, RECURSE, ps);

              // Compute "r"
              double light_normal_dot = glm::dot(light_vector, normal);
              glm::vec3 reflection_vector = glm::normalize((-1 * light_vector) + (2 * light_normal_dot * normal));

              // Get matrial
              PhongMaterial *mat = (PhongMaterial *) nearest_node->m_material;

              // Compute intensity contributions from specular and diffusion
              glm::vec3 kd_colour = get_texture(local_intersection, nearest_node) * light_normal_dot;
              glm::vec3 ks_colour = mat->m_ks * pow(glm::dot(reflection_vector, vector), mat->m_shininess);
              glm::vec3 ka_colour = ambient;

              colour += (ka_colour + ks_colour + kd_colour) * radiance;
              colour += reflection_colour;
              colour += refraction_colour;
            }
          } else {
            image(x, h-y-1, 0) = 0.2;
          }


          image(x, h-y-1, 0) += pow(colour[0], 2);
          image(x, h-y-1, 1) += pow(colour[1], 2);
          image(x, h-y-1, 2) += pow(colour[2], 2);
        }
      }

      if (perform_supersampling) {
        // Only divide by 9 if we are supersampling
        double divisor = pow(SUBPIXEL, 2);

        image(x, h-y-1, 0) /= divisor;
        image(x, h-y-1, 1) /= divisor;
        image(x, h-y-1, 2) /= divisor;
      }

      // Supersampling or not, we are going to be squarerooting our total
      image(x, h-y-1, 0) = pow(image(x, h-y-1, 0), 0.5);
      image(x, h-y-1, 1) = pow(image(x, h-y-1, 1), 0.5);
      image(x, h-y-1, 2) = pow(image(x, h-y-1, 2), 0.5);
    }
  }
}

GeometryNode *find_node_with_smallest_t(glm::vec3 src, glm::vec3 vec, SceneNode *root, glm::vec3 *intersection, glm::vec3 *local_intersection, glm::vec3 *normal, int *faceId, SceneNode *exclude, ParticleSystem *ps, bool front) {
  // Return and state variables
  double min_t = std::numeric_limits<double>::max();
  GeometryNode *min_node = NULL;
  glm::vec3 min_intersection;
  glm::vec3 min_local_intersection;
  glm::vec3 min_normal;
  int min_faceId;

  // TODO: create a new node type for CSG
  bool csg = CSG && root->m_name == "cheese";
  double min_exit_t = std::numeric_limits<double>::max();

  // Transformations
  glm::vec3 trans_src = glm::vec3(root->invtrans * glm::vec4(src, 1));
  glm::vec3 trans_vec = glm::normalize(glm::vec3(root->invtrans * glm::vec4(vec, 0)));

  // Base case
  if (root->m_nodeType == NodeType::GeometryNode && root != exclude) {
    glm::vec3 tmp_intersection;
    glm::vec3 tmp_normal;
    int tmp_faceId;
    compute_intersection(trans_src, trans_vec, (GeometryNode *) root, &tmp_intersection, &tmp_normal, &tmp_faceId, ps, front);

    double tmp_t = glm::distance(trans_src, tmp_intersection);
    if (tmp_t < min_t) {
      min_t = tmp_t;
      min_node = (GeometryNode *) root;
      min_normal = tmp_normal;
      min_intersection = tmp_intersection;
      min_local_intersection = tmp_intersection;
      min_faceId = tmp_faceId;
    }
  }

  // Recursive cases
  if (!csg || min_node != NULL) {
    for (SceneNode *child : root->children) {
      glm::vec3 tmp_intersection;
      glm::vec3 tmp_local_intersection;
      glm::vec3 tmp_normal;
      int tmp_faceId;

      GeometryNode *node = find_node_with_smallest_t(trans_src, trans_vec, child, &tmp_intersection, &tmp_local_intersection, &tmp_normal, &tmp_faceId, exclude, ps);
      double tmp_t = glm::distance(trans_src, tmp_intersection);

      if (!csg && tmp_t < min_t) {
        min_t = tmp_t;
        min_normal = tmp_normal;
        min_intersection = tmp_intersection;
        min_local_intersection = tmp_local_intersection;
        min_faceId = tmp_faceId;
        min_node = node;
      } else if (csg && node != NULL) {
        // subtractive intersection
        glm::vec3 csg_intersection;
        glm::vec3 csg_local_intersection;
        glm::vec3 csg_normal;
        int csg_faceId;

        node = find_node_with_smallest_t(trans_src, trans_vec, child, &csg_intersection, &csg_local_intersection, &csg_normal, &csg_faceId, exclude, ps, false);

        min_normal = csg_normal;
        min_intersection = csg_intersection;
        min_local_intersection = csg_local_intersection;
        min_faceId = csg_faceId;
      }
    }
  }

  if (min_node != NULL) {
    *local_intersection = min_local_intersection;
    *intersection = glm::vec3(root->trans * glm::vec4(min_intersection, 1));
    *normal = glm::normalize(glm::vec3(root->trans * glm::vec4(min_normal, 0)));
    *faceId = min_faceId;
  }

  return min_node;
}

void compute_intersection(glm::vec3 src, glm::vec3 vec, GeometryNode *node, glm::vec3 *intersection, glm::vec3 *normal, int *faceId, ParticleSystem *ps, bool front) {
  Primitive *prim = node->m_primitive;
  *faceId = -1;

  switch(prim->m_primType) {
    case PrimType::Sphere:
      compute_intersection_sphere(src, vec, intersection, normal, front);
      break;
    case PrimType::Mesh: {
      bool perform_intersection = true;
      if (BOUNDING) {
        // Create a bounding sphere of radius representative of maximal distance in object
        double estimated_radius = determine_radius((Mesh *) prim);

        glm::mat4 invtrans = glm::inverse(glm::scale(glm::vec3(estimated_radius)));
        glm::vec3 trans_src = glm::vec3(invtrans * glm::vec4(src, 1));
        glm::vec3 trans_vec = glm::normalize(glm::vec3(invtrans * glm::vec4(vec, 0)));

        // Compute intersection with sphere
        double estimated_t = std::numeric_limits<double>::max();
        compute_intersection_sphere(trans_src, trans_vec, intersection, normal);

        // Only perform actual intersection if inside
        perform_intersection = glm::distance(trans_src, *intersection) < estimated_t;
      }

      // By default always perform Mesh unless bounding volume opiton is ON
      if (perform_intersection) {
        compute_intersection_mesh(src, vec, node, intersection, normal, faceId);
      }
      break;
    }
    case PrimType::Cube:
      compute_intersection_cube(src, vec, node, intersection, normal);
      break;
    case PrimType::Particles:
      double min_t = std::numeric_limits<double>::max();
      glm::vec3 min_intersection = src + (min_t * vec);
      glm::vec3 min_normal;
      glm::mat4 min_invtrans = glm::mat4();
      double min_transparency = 0;

      if (ps->m_enable) {
        for (Particle *p : ps->m_particles) {
          // Compute translation and transform src and vec accordingly
          glm::mat4 invtrans = glm::inverse(glm::translate(p->m_velocity));
          glm::vec3 trans_src = glm::vec3(invtrans * glm::vec4(src, 1));
          glm::vec3 trans_vec = glm::normalize(glm::vec3(invtrans * glm::vec4(vec, 0)));

          glm::vec3 tmp_intersection;
          glm::vec3 tmp_normal;
          compute_intersection_sphere(trans_src, trans_vec, &tmp_intersection, &tmp_normal, front);
          double tmp_t = glm::distance(trans_src, tmp_intersection);

          // Keep note of the intersection that achieved the smallest t
          if (tmp_t < min_t) {
            min_t = tmp_t;
            min_intersection = tmp_intersection;
            min_normal = tmp_normal;
            min_invtrans = invtrans;
            min_transparency = 1 - p->m_lifespan;
          }
        }
      }

      // Undo my transformations
      glm::mat4 trans = glm::inverse(min_invtrans);
      *intersection = glm::vec3(trans * glm::vec4(min_intersection, 1));
      *normal = glm::normalize(glm::vec3(trans * glm::vec4(min_normal, 0)));

      PhongMaterial *mat = (PhongMaterial *) node->m_material;
      mat->m_transparency = min_transparency;
      break;
  }
}

void compute_intersection_sphere(glm::vec3 src, glm::vec3 vec, glm::vec3 *intersection, glm::vec3 *normal, bool front) {
  double A = 0;
  double B = 0;
  double C = 0;
  for (int i = 0; i < 3; i++) {
    A += pow(vec[i], 2);
    B += 2 * src[i] * vec[i];
    C += pow(src[i], 2);
  }
  C -= 1;

  double min_t = std::numeric_limits<double>::max();
  double determinant = pow(B,2) - (4*A*C);

  if (determinant > 0) {
    double multiplier = !front ? -1 : 1;
    min_t = (-B + multiplier * pow(determinant, 1/2))/(2*A);
  }

  *intersection = src + (min_t * vec);
  *normal = *intersection;
}

void compute_intersection_mesh(glm::vec3 src, glm::vec3 vec, GeometryNode *node, glm::vec3 *intersection, glm::vec3 *normal, int *faceId) {
  Mesh *mesh = (Mesh *) node->m_primitive;
  std::vector<glm::vec3> verts = mesh->m_vertices;
  std::vector<glm::vec3> vert_normals = mesh->m_vertex_normals;

  double min_t = std::numeric_limits<double>::max();
  glm::vec3 min_normal;
  glm::vec3 min_intersection = src + min_t * vec;
  int min_faceId = -1;

  for (int i = 0; i < mesh->m_faces.size(); i++) {
    Triangle face = mesh->m_faces[i];
    glm::vec3 v1 = verts[face.v1];
    glm::vec3 v2 = verts[face.v2];
    glm::vec3 v3 = verts[face.v3];

    glm::vec3 tmp_normal = glm::normalize(glm::cross(v1 - v2, v1 - v3));
    double tmp_t = find_line_plane_intersect(src, vec, tmp_normal, v1);

    if (tmp_t < min_t) {
      glm::vec3 q = src + tmp_t * vec;

      bool inside = on_same_triangle_side(q, v1, v2, v3) &&
        on_same_triangle_side(q, v2, v1, v3) &&
        on_same_triangle_side(q, v3, v1, v2);

      if (inside) {
        min_t = tmp_t;

        double bary[3];
        compute_barycentric_coordinates(q, v1, v2, v3, bary[0], bary[1], bary[2]);

        // For PHONG shading, we interpolate the vertex normals with barycentric coordinates
        min_normal = !PHONG ? tmp_normal : glm::normalize(bary[0] * vert_normals[face.vn1] + bary[1] * vert_normals[face.vn2] + bary[2] * vert_normals[face.vn3]);
        min_intersection = q;
        min_faceId = i;
      }
    }
  }

  *intersection = min_intersection;
  *normal = min_normal;
  *faceId = min_faceId;
}

void compute_intersection_cube(glm::vec3 src, glm::vec3 vec, GeometryNode *node, glm::vec3 *intersection, glm::vec3 *normal) {
  double min_t = std::numeric_limits<double>::max();
  glm::vec3 min_normal;

  for (int i = 0; i < 6; i++) {
    glm::vec3 p1 = cube_verts_array[cube_faces_array[(i * 4) + 0] - 1];
    glm::vec3 p2 = cube_verts_array[cube_faces_array[(i * 4) + 1] - 1];
    glm::vec3 p3 = cube_verts_array[cube_faces_array[(i * 4) + 2] - 1];
    glm::vec3 p4 = cube_verts_array[cube_faces_array[(i * 4) + 3] - 1];

    glm::vec3 tmp_normal = cube_normals[i];

    double tmp_t = find_line_plane_intersect(src, vec, tmp_normal, p1);

    // If we find that our ray intersects with a particular plane (that is not exactly at the src)
    // closer to what we have seen so far, update it
    if (tmp_t < min_t) {
      glm::vec3 q = src + (tmp_t * vec);

      if (is_inside_box(q, p1, p2, p3, p4)) {
        min_t = tmp_t;
        min_normal = tmp_normal;
      }
    }
  }
  *normal = min_normal;
  *intersection = src + (min_t * vec);

  recompute_normal_for_bump_mapping(normal, node, *intersection);
}

bool on_same_triangle_side(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
  glm::vec3 cross_product_p1 = glm::cross(p4 - p3, p1 - p3);
  glm::vec3 cross_product_p2 = glm::cross(p4 - p3, p2 - p3);

  return glm::dot(cross_product_p1, cross_product_p2) >= 0;
}

double find_line_plane_intersect(glm::vec3 src, glm::vec3 vec, glm::vec3 normal, glm::vec3 v1) {
  double d1 = glm::dot(normal, v1);
  double t1 = (d1 - glm::dot(normal, src)) / glm::dot(normal, vec);

  if (t1 > 0) {
    return t1;
  } else {
    return std::numeric_limits<double>::max();
  }
}

bool is_inside_box(glm::vec3 q, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
  glm::vec3 v1 = glm::normalize(p2 - p1);
  glm::vec3 v2 = glm::normalize(p3 - p2);
  glm::vec3 v3 = glm::normalize(p4 - p3);
  glm::vec3 v4 = glm::normalize(p1 - p4);

  glm::vec3 v5 = glm::normalize(q - p1);
  glm::vec3 v6 = glm::normalize(q - p2);
  glm::vec3 v7 = glm::normalize(q - p3);
  glm::vec3 v8 = glm::normalize(q - p4);

  return glm::dot(v1, v5) >= 0 &&
    glm::dot(v2, v6) >= 0 &&
    glm::dot(v3, v7) >= 0 &&
    glm::dot(v4, v8) >= 0;
}

double determine_radius(Mesh *mesh) {
  std::vector<glm::vec3> verts = mesh->m_vertices;

  double maximum_distance = std::numeric_limits<double>::min();
  for (glm::vec3 vert : verts) {
    for (int i = 0; i < 3; i++) {
      double absolute_value = std::abs(vert[i]);
      if (absolute_value > maximum_distance) {
        maximum_distance = absolute_value;
      }
    }
  }

  return maximum_distance * 1.1; // don't want to be right on the edge
}

void recompute_normal_for_bump_mapping(glm::vec3 *normal, GeometryNode *node, glm::vec3 intersection) {
  PhongMaterial *mat = (PhongMaterial *) node->m_material;

  if (mat->m_has_bump) {
    glm::vec2 coord;
    int index = 0;
    for (int i = 0; i < 2; i++) {
      if ((*normal)[i] == 1) {
        continue;
      }
      coord[index] = intersection[i];
      index++;
    }

    glm::vec3 bump = mat->get_pixel_from_bump(
        coord[0] * mat->m_bump_width,
        coord[1] * mat->m_bump_height);

		// pixels with a certain colour is defined to have some depth
    bump -= glm::vec3(0.5);
    *normal = glm::normalize(bump);
  }
}

glm::vec3 get_texture(glm::vec3 intersection, GeometryNode *node) {
  PhongMaterial *mat = (PhongMaterial *) node->m_material;

  if (mat->m_has_texture) {
    glm::vec2 coord = get_texture_coord_for_cube(intersection);
    glm::vec3 texture = mat->get_pixel_from_texture(
        coord[0] * mat->m_texture_width,
        coord[1] * mat->m_texture_height);
    return texture;
  }

  return mat->m_kd;
}

glm::vec2 get_texture_coord_for_cube(glm::vec3 coord) {
  int coord_index = 0;
  int texture_coord_index = 0;
  glm::vec2 texture_coord;

  while (coord_index < 3 && texture_coord_index < 2) {
    // TODO: doesn't handle the cases when coord is at corner of rectangle
    // or a face that is not facing us (but this shouldn't be a problem?)
    if (coord[coord_index] == 1) {
      coord_index++;
      continue;
    }
    texture_coord[texture_coord_index] = coord[coord_index];
    coord_index++;
    texture_coord_index++;
  }

  return texture_coord;
}
glm::vec3 generate_refraction_rays(GeometryNode *begin, glm::vec3 intersection, glm::vec3 vector, glm::vec3 normal, SceneNode *root, int depth, ParticleSystem *ps) {
  glm::vec3 refraction_colour(0, 0, 0);

  if (depth <= 0) {
    return refraction_colour;
  }

  glm::vec3 some_intersection;
  glm::vec3 some_local_intersection;
  glm::vec3 some_normal;
  int faceId;

  // Compute refraction ray
  PhongMaterial *mat = (PhongMaterial *) begin->m_material;
  double n2 = mat->m_refraction_index;
  double n1 = AIR_INDEX;
  double n = n1 / n2;

  double c1 = -1 * glm::dot(vector, normal);
  double c2 = sqrt(1 - (pow(n, 2) * (1 - pow(c1, 2))));

  glm::vec3 refracted_ray = glm::normalize((n * vector) + (n * c1 - c2) * normal);

  // Determine the nearest intersection along the refraction ray
  GeometryNode *refracted_node = find_node_with_smallest_t(intersection, refracted_ray, root, &some_intersection, &some_local_intersection, &some_normal, &faceId, begin, ps);

  if (refracted_node != NULL) {
    // Recurse
    glm::vec3 accumulated_colour = generate_refraction_rays(refracted_node, some_intersection, refracted_ray, some_normal, root, depth - 1, ps);

    PhongMaterial *r_mat = (PhongMaterial *) refracted_node->m_material;
    refraction_colour = accumulated_colour + (mat->m_transparency * get_texture(some_local_intersection, refracted_node));
  }

  return refraction_colour;
}

glm::vec3 generate_reflection_rays(GeometryNode *begin, glm::vec3 intersection, glm::vec3 vector, glm::vec3 normal, SceneNode *root, int depth, ParticleSystem *ps) {
  glm::vec3 reflection_colour(0, 0, 0);

  if (depth <= 0) {
    return reflection_colour;
  }

  glm::vec3 some_intersection;
  glm::vec3 some_local_intersection;
  glm::vec3 some_normal;
  int faceId;

  // Compute reflection ray
  glm::vec3 reflected_ray = glm::normalize(vector - (2 * normal * (glm::dot(vector, normal))));
  GeometryNode *reflected_node = find_node_with_smallest_t(intersection, reflected_ray, root, &some_intersection, &some_local_intersection, &some_normal, &faceId, begin, ps);

  if (reflected_node != NULL) {
    // Recurse
    glm::vec3 accumulated_colour = generate_reflection_rays(reflected_node, some_intersection, reflected_ray, some_normal, root, depth - 1, ps);

    PhongMaterial *mat = (PhongMaterial *) begin->m_material;
    reflection_colour = accumulated_colour + (mat->m_reflection * get_texture(some_local_intersection, reflected_node));
  }

  return reflection_colour;
}

glm::vec3 compute_incoming_radiance(Light *light, double t) {
  glm::vec3 radiance = light->colour;
  double attenuation = (light->falloff[0] + (light->falloff[1] * t) + (light->falloff[2] * pow(t, 2)));

  radiance = radiance / attenuation;
  return radiance;
}

void compute_barycentric_coordinates(glm::vec3 q, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, double &b1, double &b2, double &b3) {
  glm::vec3 v0 = p2 - p1;
  glm::vec3 v1 = p3 - p1;
  glm::vec3 v2 = q - p1;

  double d_00 = glm::dot(v0, v0);
  double d_01 = glm::dot(v0, v1);
  double d_11 = glm::dot(v1, v1);
  double d_20 = glm::dot(v2, v0);
  double d_21 = glm::dot(v2, v1);

  double divisor = d_00 * d_11 - d_01 * d_01;
  b2 = (d_11 * d_20 - d_01 * d_21) / divisor;
  b3 = (d_00 * d_21 - d_01 * d_20) / divisor;
  b1 = 1 - b2 - b3;
}
