#pragma once

#include "./include/glm/gtc/matrix_transform.hpp"
#include "OpenAssetImportMesh.h"

class COpenAssetImportMesh;
class CPlayer {
public:
    CPlayer();
    ~CPlayer();

    void Initialise(glm::vec3 position, glm::vec3 view);

    //void Accelerate(double &dt);
    void Accelerate(float acceleration, double &dt);
    void Decelerate(float acceleration, double &dt);
    void Turn(float direction, double &dt);
    void Render();

    glm::vec3 &GetPosition() { return m_position; }
    glm::vec3 &GetView() { return m_view; }
    float &GetAngle() { return m_angle; }
    float &GetAcceleration() { return m_acceleration; }
    float &GetScale() { return m_scale; }

    //used from Camera.h
    void RotateViewPoint(float angle, const glm::vec3 &viewPoint);
    void TranslateByKeyboard(double dt);
    void Advance();
    void Update(double dt);
    //-----------------------//
private:
    COpenAssetImportMesh *m_playerModel;

    float m_maxSpeed;
    float m_speed;
    float m_acceleration;

    glm::vec3 m_position;
    glm::vec3 m_view;
    glm::vec3 m_upVector;
    float m_scale;
    float m_angle;
};