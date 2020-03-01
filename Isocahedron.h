#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for cube generation, copies code from Plane.h and Sphere.h
class CIsocahedron {
public:
    CIsocahedron();
    ~CIsocahedron();
    void Create(string sDirectory, string sFilename);
    void Render();
    void Release();
private:
    UINT m_vao;
    CVertexBufferObject m_vbo;
    CTexture m_texture;
    string m_directory;
    string m_filename;
};