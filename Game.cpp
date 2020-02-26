/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 4.0a 30/01/2016
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "Player.h"
#include "Cube.h"
#include "SquarePyramid.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
    m_pPlayer = NULL;
    m_pCube = NULL;
    m_pSquarePyramid = NULL;
    m_pPlanet = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
    m_currentDistance = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
    delete m_pPlayer;
    delete m_pCube;
    delete m_pSquarePyramid;
    delete m_pPlanet;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
    m_pPlayer = new CPlayer;
    m_pCube = new CCube;
    m_pSquarePyramid = new CSquarePyramid;
    m_pPlanet = new COpenAssetImportMesh;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from https://opengameart.org/content/space-skybox-1
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	//m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013

    

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);

    

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

    m_pCatmullRom->CreateCentreline(); 
    m_pCatmullRom->CreateOffsetCurves();
    m_pCatmullRom->CreateTrack("resources\\textures\\", "track2.png");

    // Create the player
    glm::vec3 playerPosition;
    glm::vec3 playerView;
    m_pCatmullRom->Sample(0.5f, playerPosition);
    m_pCatmullRom->Sample(1.0f, playerView);
    m_pPlayer->Initialise(playerPosition, playerView); //made using Blender

    //Create the cube;
    m_pCube->Create("resources\\textures\\", "Powerup2.png", 10.0f, 10.0f, 10.0f, 5.0f);

    //Create the squarePyramid
    m_pSquarePyramid->Create("resources\\textures\\", "pyramid.jpg", 11.0f, 11.0f, 10.0f, 1.0f); //Texture by me

    m_pPlanet->Load("resources\\models\\Planet\\Planet.obj"); //made using blender
}

// Render method runs repeatedly in a loop
void Game::Render() 
{

	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();
	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("CubeMapTex", 1);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
		

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render();
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

    //render player
    modelViewMatrixStack.Push();
        pMainProgram->SetUniform("bUseTexture", true);
        m_pPlayer->GetPosition().y += sin(m_gameTime / 250)*m_pPlayer->GetSpeed() * 0.3f;
        modelViewMatrixStack.Translate(m_pPlayer->GetPosition());
        modelViewMatrixStack *= m_pPlayer->GetPlayerOrientation();
        modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f),-m_pPlayer->GetSideSpeed()* 2.0f);
        modelViewMatrixStack.Scale(m_pPlayer->GetScale());
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pPlayer->Render();
    modelViewMatrixStack.Pop();

    // Render the cube
    modelViewMatrixStack.Push();
        modelViewMatrixStack.Translate(glm::vec3(600.0f, 0.0f, -1780.0f));
        modelViewMatrixStack.Scale(2.0f);
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pCube->Render();
    modelViewMatrixStack.Pop();

    // Render the square pyramid
    modelViewMatrixStack.Push();
        modelViewMatrixStack.Translate(glm::vec3(600.0f, -360.0f + sin(m_gameTime/1000)*30.0f, -1780.0f));
        modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f,0.0f), (m_gameTime / 1000)*0.25f);
        modelViewMatrixStack.Scale(20.0f);
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pSquarePyramid->Render();
    modelViewMatrixStack.Pop();

    // Render the Planet
    modelViewMatrixStack.Push(); 
        //pMainProgram->SetUniform("bUseTexture", true);
        modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
        modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), (m_gameTime / 1000) * 0.02f);
        modelViewMatrixStack.Scale(500.0f);
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pPlanet->Render();
    modelViewMatrixStack.Pop();

    //Enabling Texture Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //render track
    modelViewMatrixStack.Push();
    pMainProgram->SetUniform("bUseTexture", true);
    pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
    pMainProgram->SetUniform("matrices.normalMatrix",
                             m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
    m_pCatmullRom->RenderTrack();
    modelViewMatrixStack.Pop();
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}


// Update method runs repeatedly with the Render method
void Game::Update()
{
    RECT dimensions = m_gameWindow.GetDimensions();

    int width = dimensions.right - dimensions.left;
    int height = dimensions.bottom - dimensions.top;

    bool playerUpdate = false;
    switch (m_pCamera->GetViewType()) {
        case 0: // Third Person
        {
            playerUpdate = true;
            break;
        }
        case 1:
        {
            playerUpdate = true;
            break;
        }
        case 2:
        {
            playerUpdate = false;
            break;
        }
    }
    float perspective = m_pPlayer->GetSpeed() < 0 ? 0 : m_pPlayer->GetSpeed() * 2.0f;
    perspective = perspective > 1.2f ? 1.2f : perspective;
    m_pCamera->SetPerspectiveProjectionMatrix(45.0f + perspective, (float)width / (float)height, 0.5f, 5000.0f);

    m_pPlayer->Update(m_dt, m_pCatmullRom->GetTrackWidth() * 0.5f, playerUpdate);
    m_currentDistance += m_pPlayer->GetSpeed();
    glm::vec3 playerPosition;
    m_pCatmullRom->Sample(m_currentDistance, playerPosition);
    glm::vec3 playerPositionNext;
    m_pCatmullRom->Sample(m_currentDistance + 1.0f, playerPositionNext);
    glm::vec3 T = glm::normalize(playerPositionNext - playerPosition);
    glm::vec3 look = playerPosition + (T * 10.0f);
    glm::vec3 up(0, 1, 0);
    glm::vec3 N = glm::normalize(glm::cross(T, up));
    glm::vec3 B = glm::normalize(glm::cross(N, T));


    glm::mat4 playerOrientation = glm::mat4(glm::mat3(-T, B, -N));
    m_pPlayer->SetPlayerOrientation(playerOrientation);
    look += m_pPlayer->GetStrafeVector();
    playerPosition += m_pPlayer->GetStrafeVector();
    playerPosition += B * 5.0f;
    m_pPlayer->Set(playerPosition, look, up);
    switch (m_pCamera->GetViewType()) {
        case 0: // Third Person
            {
                glm::vec3 cameraPosition;
                glm::vec3 cameraLook = playerPosition + (T * 20.0f * m_pPlayer->GetScale());
                cameraPosition = playerPosition;
                cameraPosition -= T * 15.0f * m_pPlayer->GetScale();
                cameraPosition += B * glm::vec3(0, 3.0f * m_pPlayer->GetScale(), 0.0f);
                m_pCamera->Set(cameraPosition, cameraLook, up);
            }
            break;
        case 1: //Top down View
            {

                glm::vec3 cameraPosition;
                glm::vec3 cameraLook = playerPosition;
                glm::vec3 cameraUp = T;
                cameraPosition = playerPosition;
                cameraPosition += B * 100.0f;
                m_pCamera->Set(cameraPosition, cameraLook, cameraUp);
            }
            break;
        case 2:
            {
                m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);
                m_pCamera->SetUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
                m_pCamera->Update(m_dt);
            }
            break;
    }
	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
    /*
    m_currentDistance += m_dt * 0.01f;
    glm::vec3 position;
    m_pCatmullRom->Sample(m_currentDistance, position);
    glm::vec3 positionNext;
    m_pCatmullRom->Sample(m_currentDistance + 1.0f, positionNext);
    glm::vec3 T = glm::normalize(positionNext - position);
    glm::vec3 look = position + (T * 10.0f);
    glm::vec3 up(0,1,0);

    glm::vec3 N = glm::normalize(glm::cross(T, up));
    glm::vec3 B = glm::normalize(glm::cross(N, T));
    position += B * glm::vec3(0, 0.5f, 0);

    m_pCamera->Set(position, look, up);
    */
	//
	/*
	static float t = 0.0f;
	t += 0.0005f * (float)m_dt;
	if (t > 1.0f)
	t = 0.0f;

	CCatmullRom cr;
	glm::vec3 p0 = glm::vec3(-500, 10, -200);
	glm::vec3 p1 = glm::vec3(0, 10, -200);
	glm::vec3 p2 = glm::vec3(0, 10, 200);
	glm::vec3 p3 = glm::vec3(-500, 10, 200);



	glm::vec3 x = cr.Interpolate(p0, p1, p2, p3, t);

	m_pCamera->Set(x, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	*/

	m_pAudio->Update();
}




void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        m_pFtFont->Print(glm::to_string(m_pCamera->GetPosition()), 20, height - 20, 20);
		//m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
    m_gameTime += m_dt;

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
        //added case to cycle camera view type
        case 0x50: //P key
            m_pCamera->CycleViewType();
            break;
        case 0x4C: //L key
            {
                m_pCamera->SetViewType(2);
            }
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
