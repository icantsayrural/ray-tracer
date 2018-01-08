#include "ParticleSystem.hpp"
#include <iostream>
ParticleSystem::ParticleSystem(bool enable) {
  m_enable = enable;
  m_acceleration = glm::vec3(0, -1, 0);
  m_decay = 0.5;
  m_num_particles = 3;
}

void ParticleSystem::add_particles() {
  // Remove all the dead particles
  std::vector<Particle *>::iterator it = m_particles.begin();
  while (it != m_particles.end()) {
    if ((*it)->is_dead()) {
      delete (*it);
      it = m_particles.erase(it);
    } else {
      it++;
    }
  }

  // Add n new ones
  for (int i = 0; i < m_num_particles; i++) {
    int dx = (int) m_dx;
    int dy = (int) m_dy;

    glm::vec3 random_velocity(rand() % dx - (dx / 2), rand() % dy, 0);
    m_particles.push_back(new Particle(random_velocity, m_acceleration, m_decay));
  }
}

void ParticleSystem::update() {
  for (Particle *p : m_particles) {
    p->update();
  }
}

void ParticleSystem::set_dims(int width, int height) {
  m_acceleration *= height;
  m_dx = 0.2 * width;
  m_dy = 0.2 * height;
}
