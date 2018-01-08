#pragma once

#include <glm/glm.hpp>

class Particle {
  public:
    Particle(glm::vec3 velocity, glm::vec3 acceleration, double decay);

    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    double m_lifespan;
    double m_decay;

    void update();
    bool is_dead();
};
