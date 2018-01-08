#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <stdlib.h>
#include "Particle.hpp"

class ParticleSystem {
  public:
    ParticleSystem(bool enable);

    std::vector<Particle *> m_particles;

    bool m_enable;
    glm::vec3 m_acceleration;
    double m_decay;
    double m_dx;
    double m_dy;
    int m_num_particles;

    void add_particles();
    void update();
    void set_dims(int width, int height);
};
