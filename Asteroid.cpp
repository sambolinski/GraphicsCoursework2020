#include "Asteroid.h"

CAsteroid::CAsteroid():CCollidable(){
    m_model = NULL;
}
CAsteroid::~CAsteroid() {
    delete m_model;
}

void CAsteroid::Initialise(glm::vec3 position, float radius, float placementOnTrack, string asteroidType) {
    m_type = "ASTEROID";
    m_asteroidType = asteroidType;
    m_position = position;
    m_radius = radius;
    m_placementOnTrack = placementOnTrack;
    m_model = new COpenAssetImportMesh;
    m_model->Load("resources\\models\\Asteroid\\Asteroid1.obj");
    m_Frame = TNBFrame();
}
void CAsteroid::UpdatePosition(glm::vec3 position) {
    m_position = position;
}

void CAsteroid::Render() {
    m_model->Render();
}