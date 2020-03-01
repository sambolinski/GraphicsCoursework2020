#include "Common.h"
#include "Isocahedron.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CIsocahedron::CIsocahedron() {
}

CIsocahedron::~CIsocahedron() {
}


// Create the cube, including its geometry, texture mapping, normal, and colour
void CIsocahedron::Create(string directory, string filename) {

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

    // Vertex positions
    //Uses vertices from this post https://stackoverflow.com/questions/20850279/incorrectly-rendering-isocahedron-in-opengl. 
    glm::vec3 isocahedronVertices[12] = {
        glm::vec3( 0.851f, 0.526f, 0.000f), //0
        glm::vec3(-0.851f, 0.526f, 0.000f), //1
        glm::vec3( 0.851f,-0.526f, 0.000f), //2
        glm::vec3(-0.851f,-0.526f, 0.000f), //3
        glm::vec3( 0.526f, 0.000f, 0.851f), //4
        glm::vec3( 0.526f, 0.000f,-0.851f), //5
        glm::vec3(-0.526f, 0.000f, 0.851f), //6
        glm::vec3(-0.526f, 0.000f,-0.851f), //7
        glm::vec3( 0.000f, 0.851f, 0.526f), //8
        glm::vec3( 0.000f,-0.851f, 0.526f), //9
        glm::vec3( 0.000f, 0.851f,-0.526f), //10
        glm::vec3( 0.000f,-0.851f,-0.526f), //11
    };
    // Triangle positions
    glm::vec3 isocahedronTriangles[60] = {
        isocahedronVertices[0],
        isocahedronVertices[10],
        isocahedronVertices[8],
        
        isocahedronVertices[8],
        isocahedronVertices[10],
        isocahedronVertices[1],

        isocahedronVertices[1],
        isocahedronVertices[10],
        isocahedronVertices[7],

        isocahedronVertices[7],
        isocahedronVertices[10],
        isocahedronVertices[5],

        isocahedronVertices[10],
        isocahedronVertices[0],
        isocahedronVertices[5],

        isocahedronVertices[5],
        isocahedronVertices[0],
        isocahedronVertices[2],

        isocahedronVertices[2],
        isocahedronVertices[0],
        isocahedronVertices[4],

        isocahedronVertices[0],
        isocahedronVertices[8],
        isocahedronVertices[4],

        isocahedronVertices[8],
        isocahedronVertices[6],
        isocahedronVertices[4],

        //mark
        isocahedronVertices[8],
        isocahedronVertices[1],
        isocahedronVertices[6],

        isocahedronVertices[6],
        isocahedronVertices[1],
        isocahedronVertices[3],

        isocahedronVertices[3],
        isocahedronVertices[1],
        isocahedronVertices[7],

        isocahedronVertices[3],
        isocahedronVertices[7],
        isocahedronVertices[11],

        isocahedronVertices[11],
        isocahedronVertices[7],
        isocahedronVertices[5],

        isocahedronVertices[11],
        isocahedronVertices[5],
        isocahedronVertices[2],

        //mark2
        isocahedronVertices[11],
        isocahedronVertices[2],
        isocahedronVertices[9],

        isocahedronVertices[2],
        isocahedronVertices[4],
        isocahedronVertices[9],

        isocahedronVertices[4],
        isocahedronVertices[6],
        isocahedronVertices[9],

        isocahedronVertices[6],
        isocahedronVertices[3],
        isocahedronVertices[9],

        isocahedronVertices[3],
        isocahedronVertices[11],
        isocahedronVertices[9],

    };
    // Texture coordinates
    glm::vec2 isocahedronTextureCoordinates[3] =
    {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.5f, 0.864f),
    };

    // Isocahedron normal
    glm::vec3 isocahedronNormals[20];
    for (unsigned int i = 0; i < 20; i++) {
        glm::vec3 A = isocahedronTriangles[(i * 3)];
        glm::vec3 B = isocahedronTriangles[(i * 3)+1];
        glm::vec3 C = isocahedronTriangles[(i * 3)+2];
        isocahedronNormals[i] = glm::normalize(glm::cross(B-A, C-A));
    }

    // Put the vertex attributes in the VBO
    unsigned int normalCounter = 0;
    for (unsigned int i = 0; i < 60; i++) {
        m_vbo.AddData(&isocahedronTriangles[i], sizeof(glm::vec3));
        m_vbo.AddData(&isocahedronTextureCoordinates[i%3], sizeof(glm::vec2));
        normalCounter += (i % 3 == 0) ? 1 : 0;
        m_vbo.AddData(&isocahedronNormals[normalCounter], sizeof(glm::vec3));
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

// Render the Isocahedron
void CIsocahedron::Render() {
    glBindVertexArray(m_vao);
    m_texture.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 60);
}

// Release resources
void CIsocahedron::Release() {
    m_texture.Release();
    glDeleteVertexArrays(1, &m_vao);
    m_vbo.Release();
}