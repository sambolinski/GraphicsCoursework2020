#pragma once
#include "Collidable.h"
#include "Cube.h"
class CCube;
class CPowerup : public CCollidable {
    CCube *m_model;
    string m_powerupType;
public:
    CPowerup();
    ~CPowerup();
    void Initialise(glm::vec3 position, float radius, float placementOnTrack, string powerupType);
    void UpdatePosition(glm::vec3 position);
    void Render() override;
    string &GetPowerupType() { return m_powerupType; }
};