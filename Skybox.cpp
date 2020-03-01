#include "Common.h"

#include "skybox.h"


CSkybox::CSkybox()
{}

CSkybox::~CSkybox()
{}


// Create a skybox of a given size with six textures
void CSkybox::Create(float size)
{
    //Skybox 1
    
    m_cubemapTexture.Create("resources\\skyboxes\\Sam\\space_rt.png", "resources\\skyboxes\\Sam\\space_lf.png",
                            "resources\\skyboxes\\Sam\\space_up.png", "resources\\skyboxes\\Sam\\space_dn.png",
                            "resources\\skyboxes\\Sam\\space_bk.png", "resources\\skyboxes\\Sam\\space_ft.png"); //Made by me using Blender
    
    //Skybox 2
    /*
    m_cubemapTexture.Create("resources\\skyboxes\\ulukai\\flipped\\corona_rt.png", "resources\\skyboxes\\ulukai\\flipped\\corona_lf.png", //Downloaded from https://opengameart.org/content/ulukais-space-skyboxes by Ulukai
                            "resources\\skyboxes\\ulukai\\flipped\\corona_up.png", "resources\\skyboxes\\ulukai\\flipped\\corona_dn.png", // on 25/2/2020
                            "resources\\skyboxes\\ulukai\\flipped\\corona_bk.png", "resources\\skyboxes\\ulukai\\flipped\\corona_ft.png"); 
    */
	
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();


	glm::vec3 vSkyBoxVertices[24] = 
	{
		// Front face
		glm::vec3(size, size, size), glm::vec3(size, -size, size), glm::vec3(-size, size, size), glm::vec3(-size, -size, size),
		// Back face
		glm::vec3(-size, size, -size), glm::vec3(-size, -size, -size), glm::vec3(size, size, -size), glm::vec3(size, -size, -size),
		// Left face
		glm::vec3(-size, size, size), glm::vec3(-size, -size, size), glm::vec3(-size, size, -size), glm::vec3(-size, -size, -size),
		// Right face
		glm::vec3(size, size, -size), glm::vec3(size, -size, -size), glm::vec3(size, size, size), glm::vec3(size, -size, size),
		// Top face
		glm::vec3(-size, size, -size), glm::vec3(size, size, -size), glm::vec3(-size, size, size), glm::vec3(size, size, size),
		// Bottom face
		glm::vec3(size, -size, -size), glm::vec3(-size, -size, -size), glm::vec3(size, -size, size), glm::vec3(-size, -size, size),
	};
	glm::vec2 vSkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 vSkyBoxNormals[6] = 
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	glm::vec4 vColour = glm::vec4(1, 1, 1, 1);
	for (int i = 0; i < 24; i++) {
		m_vbo.AddData(&vSkyBoxVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&vSkyBoxTexCoords[i%4], sizeof(glm::vec2));
		m_vbo.AddData(&vSkyBoxNormals[i/4], sizeof(glm::vec3));
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
	
}

// Render the skybox
void CSkybox::Render()
{
	glDepthMask(0);
	glBindVertexArray(m_vao);
	m_cubemapTexture.Bind(1);
	for (int i = 0; i < 6; i++) {
		//m_textures[i].Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
	}
	glDepthMask(1);
}

// Release the storage assocaited with the skybox
void CSkybox::Release()
{
	//for (int i = 0; i < 6; i++)
		//m_textures[i].Release();
	m_cubemapTexture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}