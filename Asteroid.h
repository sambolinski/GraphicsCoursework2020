#pragma once
#include "Collidable.h"
#include "OpenAssetImportMesh.h"

class COpenAssetImportMesh;
class CAsteroid : public CCollidable {
private:
    COpenAssetImportMesh *m_model;
    string m_asteroidType;
public:
    CAsteroid();
    ~CAsteroid();
    void Initialise(glm::vec3 position, float radius, float placementOnTrack, string asteroidType);
    void UpdatePosition(glm::vec3 position);
    void Render() override;
    string &GetAsteroidType() { return m_asteroidType; }
};
