#include "Collidable.h"

TNBFrame::TNBFrame() {
    T = glm::vec3(0, 0, 0);
    N = glm::vec3(0, 0, 0);
    B = glm::vec3(0, 0, 0);
}

void TNBFrame::Set(glm::vec3 t, glm::vec3 n, glm::vec3 b) {
    T = t;
    N = n;
    B = b;
}

CCollidable::CCollidable() {
}
CCollidable::~CCollidable() {

}

bool CCollidable::CheckCollision(glm::vec3 &playerPosition, float &playerScale) {
    return glm::distance(m_position, playerPosition) - playerScale <= m_radius;
}