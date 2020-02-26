#include "Common.h"
#include "SquarePyramid.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CSquarePyramid::CSquarePyramid() {
}

CSquarePyramid::~CSquarePyramid() {
}


// Create the Square Pyramid, including its geometry, texture mapping, normal, and colour
void CSquarePyramid::Create(string directory, string filename, float width, float height, float depth, float textureRepeat) {

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
    glm::vec3 squarePyramidVertices[16] = {
        //Face Bottom Done
        glm::vec3(halfWidth, -halfDepth, -halfHeight),
        glm::vec3(halfWidth, -halfDepth,  halfHeight),
        glm::vec3(-halfWidth, -halfDepth,  halfHeight),
        glm::vec3(-halfWidth, -halfDepth, -halfHeight),
        //Face Left Done
        glm::vec3(-halfWidth, -halfDepth,  halfHeight),
        glm::vec3(0,  halfDepth,  0),
        glm::vec3(-halfWidth,  -halfDepth, -halfHeight),
        //Face Front Done
        glm::vec3(halfWidth,  -halfDepth,  halfHeight),
        glm::vec3(0,  halfDepth,  0),
        glm::vec3(-halfWidth, -halfDepth,  halfHeight),
        //Face Right Done
        glm::vec3(halfWidth,  -halfDepth, -halfHeight),
        glm::vec3(0,  halfDepth,  0),
        glm::vec3(halfWidth, -halfDepth,  halfHeight),
        //Face Behind
        glm::vec3(-halfWidth,  -halfDepth, -halfHeight),
        glm::vec3(0,  halfDepth,  0),
        glm::vec3(halfWidth, -halfDepth,  -halfHeight)
    };
    unsigned int squarePyramidIndices[18] = {
        //Face Bottom
        0, 1, 2,
        0, 2, 3,

        //Face Left
        4, 5, 6,

        //Face Front
        7,  8, 9,

        //Face Right
        10, 11, 12,

        //Face Behind
        13, 14, 15
    };
    // Texture coordinates
    glm::vec2 squarePyramidFloorTexCoords[4] =
    {
        /*
        glm::vec2(textureRepeat * 0.5f, 0.0f),
        glm::vec2(textureRepeat, textureRepeat),
        glm::vec2(textureRepeat, 0.0f),
        glm::vec2(textureRepeat * 0.5f, textureRepeat),
        */
        glm::vec2(textureRepeat * 0.5f, 0.0f),
        glm::vec2(textureRepeat * 0.5f, textureRepeat),
        glm::vec2(textureRepeat, textureRepeat),
        glm::vec2(textureRepeat, 0.0f),

    };
    glm::vec2 squarePyramidSideTexCoords[3] =
    {
        glm::vec2(textureRepeat * 0.5f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(textureRepeat * 0.25f, textureRepeat)
    };

    // Cube normal
    glm::vec3 squarePyramidNormals[16] = {
        //Bottom Face
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),

        //Left Face
        glm::vec3(0.8944f, -0.4472f, 0.0f),
        glm::vec3(0.8944f, -0.4472f, 0.0f),
        glm::vec3(0.8944f, -0.4472f, 0.0f),

        //Front Face
        glm::vec3(0.0f, -0.4472f, -0.8944f),
        glm::vec3(0.0f, -0.4472f, -0.8944f),
        glm::vec3(0.0f, -0.4472f, -0.8944f),

        //Right face
        glm::vec3(-0.8944f, -0.4472f, 0.0f),
        glm::vec3(-0.8944f, -0.4472f, 0.0f),
        glm::vec3(-0.8944f, -0.4472f, 0.0f),

        //Behind Face
        glm::vec3(0.0f, -0.4472f, 0.8944f),
        glm::vec3(0.0f, -0.4472f, 0.8944f),
        glm::vec3(0.0f, -0.4472f, 0.8944f)
    };

    // Put the vertex attributes in the VBO
    unsigned int normalCounter = 0;
    unsigned int faceVertex = 4;
    for (unsigned int i = 0; i < 16; i++) {
        m_vbo.AddVertexData(&squarePyramidVertices[i], sizeof(glm::vec3));
        if (i < 4) {
            m_vbo.AddVertexData(&squarePyramidFloorTexCoords[i], sizeof(glm::vec2));
        } else {
            m_vbo.AddVertexData(&squarePyramidSideTexCoords[i % 3], sizeof(glm::vec2));
        }
        m_vbo.AddVertexData(&squarePyramidNormals[i], sizeof(glm::vec3));
    }
    for (int i = 0; i < 18; i++) {
        m_vbo.AddIndexData(&squarePyramidIndices[i], sizeof(unsigned int));
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

// Render the Pyramid
void CSquarePyramid::Render() {
    glBindVertexArray(m_vao);
    m_texture.Bind();
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
}

// Release resources
void CSquarePyramid::Release() {
    m_texture.Release();
    glDeleteVertexArrays(1, &m_vao);
    m_vbo.Release();
}