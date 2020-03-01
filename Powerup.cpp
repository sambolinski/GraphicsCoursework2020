#include "Powerup.h"

CPowerup::CPowerup() :CCollidable() {
    m_model = NULL;
}
CPowerup::~CPowerup() {
    delete m_model;
}

void CPowerup::Initialise(glm::vec3 position, float radius, float placementOnTrack, string powerupType) {
    m_type = "POWERUP";
    m_powerupType = powerupType;
    m_position = position;
    m_radius = radius;
    m_placementOnTrack = placementOnTrack;
    m_model = new CCube;

    m_model->Create("resources\\textures\\", m_type+m_powerupType+".png", 1.0f, 1.0f, 1.0f, 5.0f);
    m_Frame = TNBFrame();
}
void CPowerup::UpdatePosition(glm::vec3 position) {
    m_position = position;
}

void CPowerup::Render() {
    m_model->Render();
}