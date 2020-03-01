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
    glm::vec3 cubeVertices[8] = {
        glm::vec3(-1.0f, -1.0f, -1.0f), //0
        glm::vec3( 1.0f, -1.0f, -1.0f), //1
        glm::vec3(-1.0f,  1.0f, -1.0f), //2
        glm::vec3( 1.0f,  1.0f, -1.0f), //3
        glm::vec3(-1.0f, -1.0f,  1.0f), //4
        glm::vec3( 1.0f, -1.0f,  1.0f), //5
        glm::vec3(-1.0f,  1.0f,  1.0f), //6
        glm::vec3( 1.0f,  1.0f,  1.0f), //7
    };
    // Triangle positions
    glm::vec3 cubeTriangles[36] = {
        //Face Bottom
        cubeVertices[0],
        cubeVertices[1],
        cubeVertices[5],

        cubeVertices[5],
        cubeVertices[4],
        cubeVertices[0],

        //Face Left
        cubeVertices[1],
        cubeVertices[3],
        cubeVertices[7],

        cubeVertices[7],
        cubeVertices[5],
        cubeVertices[1],

        //Face Front
        cubeVertices[6],
        cubeVertices[4],
        cubeVertices[5],

        cubeVertices[5],
        cubeVertices[7],
        cubeVertices[6],

        //Face Right
        cubeVertices[0],
        cubeVertices[4],
        cubeVertices[6],

        cubeVertices[6],
        cubeVertices[2],
        cubeVertices[0],

        //Face Behind
        cubeVertices[0],
        cubeVertices[2],
        cubeVertices[3],

        cubeVertices[3],
        cubeVertices[1],
        cubeVertices[0],

        //Face Top
        cubeVertices[2],
        cubeVertices[6],
        cubeVertices[7],

        cubeVertices[7],
        cubeVertices[3],
        cubeVertices[2],
    };
    // Texture coordinates
    glm::vec2 cubeTexCoords[36] =
    {
        //Bottom Face
        glm::vec2(0.375f, 0.0f),
        glm::vec2(0.625f, 0.0f),
        glm::vec2(0.625f, 0.25f),

        glm::vec2(0.625f, 0.25f),
        glm::vec2(0.375f, 0.25f),
        glm::vec2(0.375f, 0.0f),

        //Face Left
        glm::vec2(0.875f, 0.750f),
        glm::vec2(0.625f, 0.750f),
        glm::vec2(0.625f, 0.50f),

        glm::vec2(0.625f, 0.50f),
        glm::vec2(0.875f, 0.50f),
        glm::vec2(0.875f, 0.750f),

        //Face Front
        glm::vec2(0.375f, 0.5f),
        glm::vec2(0.375f, 0.25f),
        glm::vec2(0.625f, 0.25f),

        glm::vec2(0.625f, 0.25f),
        glm::vec2(0.625f, 0.50f),
        glm::vec2(0.375f, 0.5f),

        //Face Right
        glm::vec2(0.125f, 0.750f),
        glm::vec2(0.125f, 0.500f),
        glm::vec2(0.375f, 0.500f),

        glm::vec2(0.375f, 0.500f),
        glm::vec2(0.375f, 0.750f),
        glm::vec2(0.125f, 0.750f),

        //Face behind
        glm::vec2(0.375f, 1.0f),
        glm::vec2(0.375f, 0.75f),
        glm::vec2(0.625f, 0.75f),

        glm::vec2(0.625f, 0.75f),
        glm::vec2(0.625f, 1.0f),
        glm::vec2(0.375f, 1.0f),

        //Face Top
        glm::vec2(0.375f, 0.750f),
        glm::vec2(0.375f, 0.5f),
        glm::vec2(0.625f, 0.5f),

        glm::vec2(0.625f, 0.5f),
        glm::vec2(0.625f, 0.75f),
        glm::vec2(0.375f, 0.75f),
    };

    // Cube normal
    glm::vec3 cubeNormals[6] = {
        //Bottom Face
        glm::vec3(0.0f, -1.0f, 0.0f),

        //Left Face
        glm::vec3(1.0f, 0.0f, 0.0f),

        //Front Face
        glm::vec3(0.0f, 0.0f, 1.0f),

        //Right face
        glm::vec3(-1.0f, 0.0f, 0.0f),

        //Behind Face
        glm::vec3(0.0f, 0.0f, -1.0f),

        //Top Face
        glm::vec3(0.0f, 1.0f,  0.0f),
    };

    // Put the vertex attributes in the VBO
    unsigned int normalCounter = 0;
    for (unsigned int i = 0; i < 36; i++) {
        m_vbo.AddData(&cubeTriangles[i], sizeof(glm::vec3));
        m_vbo.AddData(&cubeTexCoords[i], sizeof(glm::vec2));
        normalCounter += (i % 6 == 0) ? 1 : 0;
        m_vbo.AddData(&cubeNormals[normalCounter], sizeof(glm::vec3));
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
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Release resources
void CCube::Release() {
    m_texture.Release();
    glDeleteVertexArrays(1, &m_vao);
    m_vbo.Release();
}