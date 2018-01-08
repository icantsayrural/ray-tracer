#pragma once

#include <glm/glm.hpp>
#include <map>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "GeometryNode.hpp"
#include "Mesh.hpp"
#include "ParticleSystem.hpp"

void Project_Render(
    // What to render
    SceneNode * root,

    // Image to write to, set to a given width and height
    Image & image,

    // Viewing parameters
    const glm::vec3 & eye,
    const glm::vec3 & view,
    const glm::vec3 & up,
    double fovy,

    // Lighting parameters
    const glm::vec3 & ambient,
    const std::list<Light *> & lights,

    // Particle system
    ParticleSystem *ps
);

// Intersection subroutines (mostly from A4)
GeometryNode *find_node_with_smallest_t(glm::vec3 src, glm::vec3 vec, SceneNode *root, glm::vec3 *intersection, glm::vec3 *some_local_intersection, glm::vec3 *normal, int *faceId, SceneNode *exclude, ParticleSystem *ps, bool front = true);

void compute_intersection(glm::vec3 src, glm::vec3 vec, GeometryNode *node, glm::vec3 *intersection, glm::vec3 *normal, int *faceId, ParticleSystem *ps, bool front = true);
void compute_intersection_sphere(glm::vec3 src, glm::vec3 vec, glm::vec3 *intersection, glm::vec3 *normal, bool front = true);
void compute_intersection_mesh(glm::vec3 src, glm::vec3 vec, GeometryNode *node, glm::vec3 *intersection, glm::vec3 *normal, int *faceId);
void compute_intersection_cube(glm::vec3 src, glm::vec3 vec, GeometryNode *node, glm::vec3 *intersection, glm::vec3 *normal);

bool on_same_triangle_side(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
double find_line_plane_intersect(glm::vec3 src, glm::vec3 vec, glm::vec3 normal, glm::vec3 v1);
bool is_inside_box(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
double determine_radius(Mesh *mesh);

// Bump and texture mapping subroutines
void recompute_normal_for_bump_mapping(glm::vec3 *normal, GeometryNode *node, glm::vec3 intersection);
glm::vec3 get_texture(glm::vec3 intersection, GeometryNode *node);
glm::vec2 get_texture_coord_for_cube(glm::vec3 coord);

// Secondary ray subroutines
glm::vec3 generate_refraction_rays(GeometryNode *begin, glm::vec3 intersection, glm::vec3 vector, glm::vec3 normal, SceneNode *root, int depth, ParticleSystem *ps);
glm::vec3 generate_reflection_rays(GeometryNode *begin, glm::vec3 intersection, glm::vec3 vector, glm::vec3 normal, SceneNode *root, int depth, ParticleSystem *ps);

// Lighting subroutines
glm::vec3 compute_incoming_radiance(Light *light, double t);

// Phong shading subroutines
void compute_barycentric_coordinates(glm::vec3 q, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, double &b1, double &b2, double &b3);
