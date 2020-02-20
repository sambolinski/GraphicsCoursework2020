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
    void AccelerateSide(float acceleration, double &dt);
    void DecelerateSide(float acceleration, double &dt);
    void Turn(float direction, double &dt);
    void Render();

    glm::vec3 &GetPosition() { return m_position; }
    glm::vec3 &GetView() { return m_view; }
    float &GetAngle() { return m_angle; }
    float &GetAcceleration() { return m_acceleration; }
    float &GetScale() { return m_scale; }
    float &GetSpeed() { return m_speed;  }
    glm::vec3 &GetStrafeVector() { return m_strafeVector; }
    float &GetSideAmount() { return m_sideAmount; }
    float &GetSideSpeed() { return m_sideSpeed; }
    void SetView(glm::vec3 view) { m_view = view; }
    glm::mat4 &GetPlayerOrientation() { return m_playerOrientation; }
    void SetPlayerOrientation(glm::mat4 playerOrientation) { m_playerOrientation = playerOrientation; };

    //used from Camera.h
    void RotateViewPoint(float angle, const glm::vec3 &viewPoint);
    void TranslateByKeyboard(double dt);
    void Advance();
    void Strafe(double max);
    void Update(double dt, double max);
    void Set(glm::vec3 &position, glm::vec3 &viewpoint, glm::vec3 &upVector);
    //-----------------------//
private:
    COpenAssetImportMesh *m_playerModel;

    //forward speed
    float m_maxSpeed;
    float m_speed;
    //sideways speed
    float m_maxSideSpeed;
    float m_sideSpeed;
    float m_acceleration;

    float m_sideAmount = 0;
    glm::vec3 m_position;
    glm::vec3 m_view;
    glm::vec3 m_upVector;
    glm::vec3 m_strafeVector;
    glm::mat4 m_playerOrientation;
    float m_scale;
    float m_angle;
};