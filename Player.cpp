#include "Player.h"

CPlayer::CPlayer() {
    m_playerModel = NULL;
    m_shieldModel = NULL;
    m_audio = NULL;
    m_maxSpeed = 0.0f;
    m_minSpeed = 0.0f;
    m_speed = 0.0f;
    m_maxSideSpeed = 0.0f;
    m_sideSpeed = 0.0f;
    m_acceleration = 0.0f;

    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_view = glm::vec3(0.0f, 0.0f, 0.0f);
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    m_strafeVector = glm::vec3(0.00f, 0.0f, 0.0f);
    m_angle = 0;
    m_scale = 0;

    m_shield = false;
    m_isBoosting = false;
    m_boost = 0.0f;
    m_boostAcceleration = 0.0f;
    m_timeBoosting = 0;
    m_numBoosts = 0;
    m_health = 0;
    m_maxBoost = 0;
}

CPlayer::~CPlayer() {
    delete m_playerModel;
    delete m_shieldModel;
}

void CPlayer::Initialise(glm::vec3 position, glm::vec3 view) {
    m_playerModel = new COpenAssetImportMesh;
    m_playerModel->Load("resources\\models\\Saucer\\Saucer.obj");
    m_shieldModel = new CIsocahedron;
    m_shieldModel->Create("resources\\textures\\", "ShieldGreen.png");
    m_maxSpeed = 0.045 * 20.0f; //laptop  = *2, pc = *10
    m_minSpeed = m_maxSpeed * 0.5f;
    m_maxSideSpeed = 0.045f * 10;
    m_acceleration = 0.0001f * 4.0f;
    m_position = position;
    m_view = view;

    //starting angle
    glm::vec3 base = (position - glm::vec3(0, 0, 0));
    m_angle = 0.5f*3.141f;//90 degrees radians will ahve to change acosf(glm::dot(base, (m_view)) / (glm::length(base)*glm::length((m_view))));

    m_scale = 1.0f;
    m_shield = false;
    m_boost = 1.0f;
    m_timeBoosting = 0;
    m_numBoosts = 0;
    m_boostAcceleration = 0.01f;
    m_health = 1;
    m_maxBoost = 3000;
}
void CPlayer::Accelerate(float acceleration, double &dt) {
    //ternery statement - adds acceleration to speed unless max speed is reached
    m_speed = m_speed + (acceleration * dt) <= m_maxSpeed * m_boost ? m_speed + (acceleration * dt) : m_maxSpeed * m_boost;
    //ternery statement - checks max speed is greater than minimum
    m_speed = m_speed + (acceleration * dt) >= m_minSpeed ? m_speed + (acceleration * dt) : m_minSpeed;
}
void CPlayer::AccelerateSide(float acceleration, double &dt) {
    //ternery statement - adds acceleration to speed unless max speed is reached
    m_sideSpeed = m_sideSpeed + (acceleration * 1.2f * dt) <= m_maxSideSpeed ? m_sideSpeed + (acceleration * dt) : m_maxSideSpeed;
    //ternery statement - checks max speed is greater than minimum
    m_sideSpeed = m_sideSpeed + (acceleration * 1.2f * dt) >= m_maxSideSpeed * -1.0f ? m_sideSpeed + (acceleration * dt) : m_maxSideSpeed * -1.0f;
}

void CPlayer::Decelerate(float acceleration, double &dt) {
    if (acceleration > 0) {
        m_speed = m_speed + (acceleration * dt) <= 0 ? m_speed + (acceleration * dt) : 0;
    } else if(acceleration < 0){
        m_speed = m_speed + (acceleration * dt) >= 0 ? m_speed + (acceleration * dt) : 0;
    }
}
void CPlayer::DecelerateSide(float acceleration, double &dt) {
    if (acceleration > 0) {
        m_sideSpeed = m_sideSpeed + (acceleration * dt) <= 0 ? m_sideSpeed + (acceleration * dt) : 0;
    } else if(acceleration < 0){
        m_sideSpeed = m_sideSpeed + (acceleration * dt) >= 0 ? m_sideSpeed + (acceleration * dt) : 0;
    }
}

void CPlayer::Advance(double dt) {
    glm::vec3 view = glm::normalize(m_view - m_position);
    m_position = m_position + view * (m_speed * (float)dt * 0.1f);
    m_view = m_view + view * m_speed;

}
void CPlayer::Update(double dt, double max, bool playerUpdate) {

    glm::vec3 vector = glm::cross(m_view - m_position, m_upVector);
    m_strafeVector = glm::normalize(vector);
    if (playerUpdate) {
        TranslateByKeyboard(dt);
    } else {
        if (m_speed > m_minSpeed) {
            Decelerate(m_acceleration*-4.0f, dt);
        } else if (m_speed < m_minSpeed) {
            Accelerate(m_acceleration, dt);
        }
        if (m_sideSpeed < 0.0f) {
            DecelerateSide(m_acceleration * 1.0f, dt);
        } else if (m_sideSpeed > 0.0f) {
            DecelerateSide(m_acceleration * -1.0f, dt);
        }
    }
    if (m_isBoosting) {
        m_timeBoosting += dt;
    }
    Advance(dt);
    Strafe(max);
}
void CPlayer::Strafe( double max) {
    if (m_sideAmount > max) {
        m_sideAmount = max;
    } else if(m_sideAmount < -max) {
        m_sideAmount = -max;
    }
    if (m_sideAmount <= max && m_sideAmount >= -max) {
        m_sideAmount += m_sideSpeed;
    }
    if (m_sideAmount > 0) {
        m_strafeVector *= max;
        m_strafeVector *= (m_sideAmount / max);
    } else {
        m_strafeVector *= -max;
        m_strafeVector *= (m_sideAmount / -max);

    }
}
void CPlayer::TranslateByKeyboard(double dt) {
    if (GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {
        Accelerate(m_acceleration * m_boost, dt);
    } else {
        if (m_speed > m_minSpeed) {
            Decelerate(m_acceleration*-4.0f, dt);
        }
    }

    if (GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {
        Accelerate(m_acceleration * -1.0f, dt);
    } else {
        if (m_speed < m_minSpeed) {
            Accelerate(m_acceleration, dt);
        }
    }

    if (GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {
        AccelerateSide(m_acceleration * -1.0f, dt);
    } else {
        if (m_sideSpeed < 0.0f) {
            DecelerateSide(m_acceleration * 1.0f, dt);
        }
    }

    if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {
        AccelerateSide(m_acceleration, dt);
    } else {
        if (m_sideSpeed > 0.0f) {
            DecelerateSide(m_acceleration * -1.0f, dt);
        }
    }

    if (GetKeyState('B') & 0x80) {
        ActivateBoost();
    }
}
void CPlayer::Set(glm::vec3 &position, glm::vec3 &viewpoint, glm::vec3 &upVector) {
    m_position = position;
    m_view = viewpoint;
    m_upVector = upVector;

}
void CPlayer::Render() {
    m_playerModel->Render();
}

void CPlayer::RenderShield() {
    if (m_shield) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_shieldModel->Render();
    }
}

void CPlayer::ActivateBoost() {
    if (m_numBoosts > 0) {
        if (m_isBoosting == false) {
            m_isBoosting = true;
            m_timeBoosting = 0;
            m_numBoosts--;
            //m_audio->LoadEventSound("Resources\\Audio\\boost.wav"); //https://freesound.org/people/Hilfsabeitah/sounds/478592/ by “Hilfsabeitah” on 09/03/2020 (CC0 1.0 Universal (CC0 1.0) Public Domain Dedication)
            //m_audio->PlayEventSound();
        }
    }
}
void CPlayer::BoostAcceleration(double dt) {
    if (m_timeBoosting > m_maxBoost) {
        m_isBoosting = false;
        m_timeBoosting = 0.0f;
    }
    if (m_isBoosting) {
        m_boost = m_boost + (m_boostAcceleration * dt) <= 5.0f ? m_boost + (m_boostAcceleration * dt) : 5.0f;
    } else {
        m_boost = m_boost - (m_boostAcceleration * dt) >= 1.0f ? m_boost - (m_boostAcceleration * dt) : 1.0f;
    }
}

void CPlayer::Reset() {
    m_isBoosting = false;
    m_boost = 1.0f;
    m_shield = false;
    m_timeBoosting = 0;
    m_numBoosts = 0;
    m_speed = 0.0f;
    m_sideSpeed = 0;
    m_health = 1;
}

void CPlayer::TakeDamage() {
    if (m_health > 0) {
        m_health--;
    }
}