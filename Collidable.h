#pragma once
#include "Common.h"

struct TNBFrame{
public:
    TNBFrame();
    void Set(glm::vec3 t, glm::vec3 n, glm::vec3 b);
    glm::vec3 T;
    glm::vec3 N;
    glm::vec3 B;
};
class CCollidable {
protected:
    glm::vec3 m_position;
    float m_radius;
    float m_placementOnTrack;
    glm::mat4 m_playerOrientation;
    string m_type;
    TNBFrame m_Frame;
public:
    CCollidable();
    ~CCollidable();
    bool CheckCollision(glm::vec3 &playerPosition, float &playerScale);
    virtual void Render() = 0;

    glm::vec3 &GetPosition() { return m_position; }
    string &GetType() { return m_type; }
    float &GetRadius() { return m_radius; }
    float &GetPlacementOnTrack() { return m_placementOnTrack; }
    TNBFrame &GetTNBFrame() { return m_Frame; }
    void SetPlayerOrientation(glm::mat4 playerOrientation) { m_playerOrientation = playerOrientation; }
    glm::mat4 &GetPlayerOrientation() { return m_playerOrientation; }
    void SetPosition(glm::vec3 position) { m_position = position; }
    void SetPreviousGameTime(float time) { m_previousGameTime = time; }
    float &GetPreviousGameTime() { return m_previousGameTime; }
private:
    float m_previousGameTime = 0;
};