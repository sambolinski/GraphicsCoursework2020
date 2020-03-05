#pragma once

#include "./include/glm/gtc/matrix_transform.hpp"
#include "OpenAssetImportMesh.h"
#include "Isocahedron.h"

class COpenAssetImportMesh;
class CIsocahedron;
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
    void Render();
    void RenderShield();

    glm::vec3 &GetPosition() { return m_position; }
    glm::vec3 &GetView() { return m_view; }
    float &GetAngle() { return m_angle; }
    float &GetAcceleration() { return m_acceleration; }
    float &GetScale() { return m_scale; }
    float &GetSpeed() { return m_speed;  }
    float &GetMaxSpeed() { return m_maxSpeed; }
    glm::vec3 &GetStrafeVector() { return m_strafeVector; }
    float &GetSideAmount() { return m_sideAmount; }
    float &GetSideSpeed() { return m_sideSpeed; }
    void SetView(glm::vec3 view) { m_view = view; }
    glm::mat4 &GetPlayerOrientation() { return m_playerOrientation; }
    void SetPlayerOrientation(glm::mat4 playerOrientation) { m_playerOrientation = playerOrientation; }
    float &GetBoost() { return m_boost; }
    unsigned int &GetNumBoost() { return m_numBoosts; }
    unsigned int &GetHealth() { return m_health; }
    bool &GetShield() { return m_shield; }
    double &GetTimeBoosting() { return m_timeBoosting; }
    float &GetMaxBoost() { return m_maxBoost; }
    void ActivateShield() { m_shield = true; }
    void DeactivateShield() { m_shield = false; }
    void IncrementBoost() { m_numBoosts + 1 < 2?m_numBoosts++:1; };
    void ActivateBoost();
    void BoostAcceleration(double dt);
    void Reset();
    void TakeDamage();

    //used from Camera.h but updated for player
    void TranslateByKeyboard(double dt);
    void Advance(double dt);
    void Strafe(double max);
    void Update(double dt, double max, bool playerUpdate);
    void Set(glm::vec3 &position, glm::vec3 &viewpoint, glm::vec3 &upVector);
    //-----------------------//
private:
    COpenAssetImportMesh *m_playerModel;
    CIsocahedron *m_shieldModel;

    //forward speed
    float m_maxSpeed;
    float m_minSpeed;
    float m_speed;
    //sideways speed
    float m_maxSideSpeed;
    float m_sideSpeed;

    float m_acceleration;

    float m_sideAmount = 0;

    //powerups
        //Boost
    unsigned int m_numBoosts;
    float m_boost;
    bool m_isBoosting;
    float m_boostAcceleration;
    double m_timeBoosting;
        //Shield
    bool m_shield;

    //health
    unsigned int m_health;
    glm::vec3 m_position;
    glm::vec3 m_view;
    glm::vec3 m_upVector;
    glm::vec3 m_strafeVector;
    glm::mat4 m_playerOrientation;
    float m_scale;
    float m_angle;
    float m_maxBoost;
};