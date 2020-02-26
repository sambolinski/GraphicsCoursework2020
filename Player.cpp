#include "Player.h"

CPlayer::CPlayer() {
    m_playerModel = NULL;

    m_maxSpeed = 0.0f;
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
}

CPlayer::~CPlayer() {
    delete m_playerModel;
}

void CPlayer::Initialise(glm::vec3 position, glm::vec3 view) {
    m_playerModel = new COpenAssetImportMesh;
    m_playerModel->Load("resources\\models\\Saucer\\Saucer.obj");
    m_maxSpeed = 0.045 * 50.0f; //laptop  = *2, pc = *10
    m_maxSideSpeed = 0.045f * 2;
    m_acceleration = 0.0001f * 1.0f;
    m_position = position;
    m_view = view;

    //starting angle
    glm::vec3 base = (position - glm::vec3(0, 0, 0));
    m_angle = 0.5f*3.141f;//90 degrees radians will ahve to change acosf(glm::dot(base, (m_view)) / (glm::length(base)*glm::length((m_view))));

    m_scale = 1.0f;
}

/*
void CPlayer::Accelerate(double &dt) {
    //ternery statement - adds acceleration to speed unless max speed is reached
    m_speed = m_speed + (m_acceleration*dt) >= m_maxSpeed ? m_speed + (m_acceleration*dt) : m_maxSpeed;
}
*/
void CPlayer::Accelerate(float acceleration, double &dt) {
    //ternery statement - adds acceleration to speed unless max speed is reached
    m_speed = m_speed + (acceleration * dt) <= m_maxSpeed ? m_speed + (acceleration * dt) : m_maxSpeed;
    //ternery statement - checks max speed is greater than minimum
    m_speed = m_speed + (acceleration * dt) >= m_maxSpeed*-1.0f ? m_speed + (acceleration * dt) : m_maxSpeed*-1.0f;
}
void CPlayer::AccelerateSide(float acceleration, double &dt) {
    //ternery statement - adds acceleration to speed unless max speed is reached
    m_sideSpeed = m_sideSpeed + (acceleration * dt) <= m_maxSideSpeed ? m_sideSpeed + (acceleration * dt) : m_maxSideSpeed;
    //ternery statement - checks max speed is greater than minimum
    m_sideSpeed = m_sideSpeed + (acceleration * dt) >= m_maxSideSpeed * -1.0f ? m_sideSpeed + (acceleration * dt) : m_maxSideSpeed * -1.0f;
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

void CPlayer::Turn(float direction, double &dt) {
    float oldAngle = m_angle;
    m_angle += direction * dt * 0.0015f;
    RotateViewPoint((m_angle-oldAngle)/(180.0f/3.141f), glm::vec3(0.0f, 1.0f, 0.0f));
}

//taken from Camera.h
void CPlayer::RotateViewPoint(float fAngle, const glm::vec3 &vPoint) {
    glm::vec3 vView = m_view - m_position;

    glm::mat4 R = glm::rotate(glm::mat4(1), fAngle * 180.0f / (float)M_PI, vPoint);
    glm::vec4 newView = R * glm::vec4(vView, 1);

    m_view = m_position + glm::vec3(newView);
}

void CPlayer::Advance() {
    glm::vec3 view = glm::normalize(m_view - m_position);
    m_position = m_position + view * m_speed;
    m_view = m_view + view * m_speed;

}
void CPlayer::Update(double dt, double max, bool playerUpdate) {

    glm::vec3 vector = glm::cross(m_view - m_position, m_upVector);
    m_strafeVector = glm::normalize(vector);
    if (playerUpdate) {
        TranslateByKeyboard(dt);
    } else {
        if (m_speed > 0.0f) {
            Decelerate(m_acceleration*-4.0f, dt);
        } else if (m_speed < 0.0f) {
            Decelerate(m_acceleration, dt);
        }


        if (m_sideSpeed < 0.0f) {
            DecelerateSide(m_acceleration * 1.0f, dt);
        } else if (m_sideSpeed > 0.0f) {
            DecelerateSide(m_acceleration * -1.0f, dt);
        }
    }
    Advance();
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
        Accelerate(m_acceleration, dt);
    } else {
        if (m_speed > 0.0f) {
            Decelerate(m_acceleration*-4.0f, dt);
        }
    }

    if (GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {
        Accelerate(m_acceleration * -1.0f, dt);
    } else {
        if (m_speed < 0.0f) {
            Decelerate(m_acceleration, dt);
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
}
void CPlayer::Set(glm::vec3 &position, glm::vec3 &viewpoint, glm::vec3 &upVector) {
    m_position = position;
    m_view = viewpoint;
    m_upVector = upVector;

}
void CPlayer::Render() {
    m_playerModel->Render();
}