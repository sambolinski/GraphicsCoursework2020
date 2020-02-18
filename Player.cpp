#include "Player.h"

CPlayer::CPlayer() {
    m_playerModel = NULL;

    m_maxSpeed = 0.0f;
    m_speed = 0.0f;
    m_acceleration = 0.0f;

    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_view = glm::vec3(0.0f, 0.0f, 0.0f);
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    m_angle = 0;
    m_scale = 0;
}

CPlayer::~CPlayer() {
    delete m_playerModel;
}

void CPlayer::Initialise(glm::vec3 position, glm::vec3 view) {
    m_playerModel = new COpenAssetImportMesh;
    m_playerModel->Load("resources\\models\\Horse\\Horse2.obj");
    m_maxSpeed = 0.045f;
    m_acceleration = 0.0001f;
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
    m_speed = m_speed + (acceleration * dt) >= m_maxSpeed*-1.0f ? m_speed + (acceleration * dt) : m_maxSpeed*1.0f;
}

void CPlayer::Decelerate(float acceleration, double &dt) {
    if (acceleration > 0) {
        m_speed = m_speed + (acceleration * dt) >= 0 ? m_speed + (acceleration * dt) : 0;
    } else {
        m_speed = m_speed + (acceleration * dt) <= 0 ? m_speed + (acceleration * dt) : 0;
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
void CPlayer::Update(double dt) {
    TranslateByKeyboard(dt);
    Advance();
}

void CPlayer::TranslateByKeyboard(double dt) {
    if (GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {
        Accelerate(m_acceleration, dt);
    } else {
        if (m_speed > 0.0f) {
            Decelerate(m_acceleration*-1.0f, dt);
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
        Turn(1.0f, dt);
    }

    if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {
        Turn(-1.0f, dt);
    }
}
void CPlayer::Render() {
    m_playerModel->Render();
}