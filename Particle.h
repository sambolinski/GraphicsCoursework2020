#pragma once
#include "Common.h"

class CParticle {
public:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_lifetime;
    float m_maxLifetime;

    CParticle() {
        m_position = glm::vec3(0, 0, 0);
        m_velocity = glm::vec3(0, 0, 0);
        m_lifetime = 0;
        m_maxLifetime = 10;
    }



};