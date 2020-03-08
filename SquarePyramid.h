#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for cube generation, copies code from Plane.h and Sphere.h
class CSquarePyramid {
public:
    CSquarePyramid();
    ~CSquarePyramid();
    void Create(string sDirectory, string sFilename, float fWidth, float fHeight, float fDepth, float fTextureRepeat);
    void Render();
    void Release();
private:
    UINT m_vao;
    CVertexBufferObject m_vbo;
    CTexture m_texture;
    string m_directory;
    string m_filename;
    float m_width;
    float m_height;
    float m_depth;
};
