#include "Common.h"
#include "Cube.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CCube::CCube() {
}

CCube::~CCube() {
}


// Create the cube, including its geometry, texture mapping, normal, and colour
void CCube::Create(string directory, string filename, float width, float height, float depth, float textureRepeat) {

    m_width = width;
    m_height = height;
    m_depth = depth;

    // Load the texture
    m_texture.Load(directory + filename, true);

    m_directory = directory;
    m_filename = filename;

    // Set parameters for texturing using sampler object
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);


    // Use VAO to store state associated with vertices
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create a VBO
    m_vbo.Create();
    m_vbo.Bind();

    float halfWidth = m_width / 2.0f;
    float halfHeight = m_height / 2.0f;
    float halfDepth = m_depth / 2.0f;

    // Vertex positions
    glm::vec3 cubeVertices[24] = {
        //Face Bottom
        glm::vec3(halfWidth, -halfDepth, -halfHeight),
        glm::vec3(halfWidth, -halfDepth,  halfHeight),
        glm::vec3(-halfWidth, -halfDepth,  halfHeight),
        glm::vec3(-halfWidth, -halfDepth, -halfHeight),
        //Face Left
        glm::vec3(-halfWidth, -halfDepth,  halfHeight),
        glm::vec3(-halfWidth,  halfDepth,  halfHeight),
        glm::vec3(-halfWidth,  halfDepth, -halfHeight),
        glm::vec3(-halfWidth, -halfDepth, -halfHeight),
        //Face Front
        glm::vec3(halfWidth, -halfDepth,  halfHeight),
        glm::vec3(halfWidth,  halfDepth,  halfHeight),
        glm::vec3(-halfWidth,  halfDepth,  halfHeight),
        glm::vec3(-halfWidth, -halfDepth,  halfHeight),
        //Face Right
        glm::vec3(halfWidth, -halfDepth, -halfHeight),
        glm::vec3(halfWidth,  halfDepth, -halfHeight),
        glm::vec3(halfWidth,  halfDepth,  halfHeight),
        glm::vec3( halfWidth, -halfDepth,  halfHeight),
        //Face Behind
        glm::vec3(-halfWidth, -halfDepth, -halfHeight),
        glm::vec3(-halfWidth,  halfDepth, -halfHeight),
        glm::vec3( halfWidth,  halfDepth, -halfHeight),
        glm::vec3( halfWidth, -halfDepth, -halfHeight),
        //Face Top
        glm::vec3(-halfWidth,  halfDepth, -halfHeight),
        glm::vec3(-halfWidth,  halfDepth,  halfHeight),
        glm::vec3( halfWidth,  halfDepth,  halfHeight),
        glm::vec3( halfWidth,  halfDepth, -halfHeight)
    };
    unsigned int cubeIndices[36] = {
        //Face Bottom
        0, 1, 2,
        0, 2, 3,

        //Face Left
        4, 5, 6,
        4, 6, 7,

        //Face Front
        8,  9, 10,
        8, 10, 11,

        //Face Right
        12, 13, 14,
        12, 14, 15,

        //Face Behind
        16, 17, 18,
        16, 18, 19,

        //Face Top
        20, 21, 22,
        20, 22, 23,

    };
    // Texture coordinates
    glm::vec2 cubeTexCoords[24] =
    {
        /*
        //Bottom Face
        glm::vec2(0.625f, 0.25f),
        glm::vec2(0.625f, 0.0f),
        glm::vec2(0.375f, 0.25f),
        glm::vec2(0.375f, 0.0f),

        //Face Left
        glm::vec2(0.375f, 0.50f),
        glm::vec2(0.125f, 0.50f),
        glm::vec2(0.375f, 0.75f),
        glm::vec2(0.125f, 0.75f),

        //Face Front
        glm::vec2(0.375f, 0.75f),
        glm::vec2(0.375f, 1.0f),
        glm::vec2(0.625f, 0.75f),
        glm::vec2(0.625f, 1.0f),

        //Face Right
        glm::vec2(0.625f, 0.5f),
        glm::vec2(0.625f, 0.75f),
        glm::vec2(0.875f, 0.5f),
        glm::vec2(0.875f, 0.75f),

        //Face behind
        glm::vec2(0.375f, 0.25f),
        glm::vec2(0.375f, 0.5f),
        glm::vec2(0.625f, 0.25f),
        glm::vec2(0.625f, 0.5f),
        */
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        //Face Top
        glm::vec2(0.375f, 0.5f),
        glm::vec2(0.375f, 0.25f),
        glm::vec2(0.625f, 0.5f),
        glm::vec2(0.625f, 0.25f)
    };

    // Cube normal
    glm::vec3 cubeNormals[6] = {
        //Bottom Face
        glm::vec3(0.0f, 1.0f, 0.0f),

        //Left Face
        glm::vec3(1.0f, 0.0f, 0.0f),

        //Front Face
        glm::vec3(0.0f, 0.0f, -1.0f),

        //Right face
        glm::vec3(-1.0f, 0.0f, 0.0f),

        //Behind Face
        glm::vec3(0.0f, 0.0f, 1.0f),

        //Top Face
        glm::vec3(0.0f, -1.0f,  0.0f),
    };

    // Put the vertex attributes in the VBO
    unsigned int normalCounter = 0;
    for (unsigned int i = 0; i < 24; i++) {
        m_vbo.AddVertexData(&cubeVertices[i], sizeof(glm::vec3));
        m_vbo.AddVertexData(&cubeTexCoords[i], sizeof(glm::vec2));
        normalCounter += (i % 4 == 0) ? 1 : 0;
        m_vbo.AddVertexData(&cubeNormals[normalCounter], sizeof(glm::vec3));
    }
    for (int i = 0; i < 36; i++) {
        m_vbo.AddIndexData(&cubeIndices[i], sizeof(unsigned int));
    }


    // Upload the VBO to the GPU
    m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

    // Set the vertex attribute locations
    GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

// Render the cube
void CCube::Render() {
    glBindVertexArray(m_vao);
    m_texture.Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// Release resources
void CCube::Release() {
    m_texture.Release();
    glDeleteVertexArrays(1, &m_vao);
    m_vbo.Release();
}