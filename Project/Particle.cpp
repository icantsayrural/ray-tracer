#include "Particle.hpp"
#include <iostream>
#include "glm/ext.hpp"

Particle::Particle(glm::vec3 velocity, glm::vec3 acceleration, double decay): m_velocity(velocity), m_acceleration(acceleration), m_lifespan(1), m_decay(decay) {
}

void Particle::update() {
  m_velocity += m_velocity + m_acceleration; // we want to preserve previous displacement
  m_lifespan -= m_decay;
}

bool Particle::is_dead() {
  return m_lifespan < 0;
}
