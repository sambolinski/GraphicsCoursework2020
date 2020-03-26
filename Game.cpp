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
#include <random>
#include <chrono>

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
#include "Collidable.h"
#include "Asteroid.h"
#include "Powerup.h"
#include "Isocahedron.h"
#include "FrameBufferObject.h"
#include "Particle.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pGUI = NULL;
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
    m_collidableObjects = NULL;
    m_pIsocahedron = NULL;
    m_pFBO = NULL;
    m_pFrameBufferWindow = NULL;
    m_asteroids = NULL;
    m_asteroidLocations = NULL;
    m_pAsteroid = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
    m_currentDistance = 0.0f;
    m_maxLaps = 0;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pGUI;
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
    delete m_pIsocahedron;
    delete m_pFBO;
    delete m_pFrameBufferWindow;
    delete m_pAsteroid;

    if (m_collidableObjects != NULL) {
        for (unsigned int i = 0; i < m_collidableObjects->size(); i++)
            delete (*m_collidableObjects)[i];
    }
    delete m_collidableObjects;    

    if (m_asteroids != NULL) {
        for (unsigned int i = 0; i < m_asteroids->size(); i++)
            delete (*m_asteroids)[i];
    }
    delete m_asteroids;

    if (m_asteroidLocations != NULL) {
        for (unsigned int i = 0; i < m_asteroidLocations->size(); i++)
            delete (*m_asteroidLocations)[i];
    }
    delete m_asteroidLocations;

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
	m_pGUI = new CPlane;
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
    m_pIsocahedron = new CIsocahedron;
    m_collidableObjects = new vector <CCollidable *>;
    m_pFBO = new CFrameBufferObject;
    m_pFrameBufferWindow = new CPlane;
    m_asteroids = new vector<COpenAssetImportMesh *>;
    m_asteroidLocations = new vector<glm::mat4 *>;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

    m_maxLaps = 3;
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
    sShaderFileNames.push_back("guiBackgroundShader.vert");
    sShaderFileNames.push_back("guiBackgroundShader.frag");
    sShaderFileNames.push_back("postProcessing.vert");
    sShaderFileNames.push_back("postProcessing.frag");
    sShaderFileNames.push_back("planetRing.vert");
    sShaderFileNames.push_back("planetRing.frag");
    sShaderFileNames.push_back("particleShader.vert");
    sShaderFileNames.push_back("particleShader.frag");

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

    //MultiSampling
    glEnable(GL_MULTISAMPLE);

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

	// Create a shader program for background GUI
    CShaderProgram *pGUIShader = new CShaderProgram;
    pGUIShader->CreateProgram();
    pGUIShader->AddShaderToProgram(&shShaders[4]);
    pGUIShader->AddShaderToProgram(&shShaders[5]);
    pGUIShader->LinkProgram();
    m_pShaderPrograms->push_back(pGUIShader);

    CShaderProgram *pPostProcessingShader = new CShaderProgram;
    pPostProcessingShader->CreateProgram();
    pPostProcessingShader->AddShaderToProgram(&shShaders[6]);
    pPostProcessingShader->AddShaderToProgram(&shShaders[7]);
    pPostProcessingShader->LinkProgram();
    m_pShaderPrograms->push_back(pPostProcessingShader);    
    
    CShaderProgram *pPlanetRingShader = new CShaderProgram;
    pPlanetRingShader->CreateProgram();
    pPlanetRingShader->AddShaderToProgram(&shShaders[8]);
    pPlanetRingShader->AddShaderToProgram(&shShaders[9]);
    pPlanetRingShader->LinkProgram();
    m_pShaderPrograms->push_back(pPlanetRingShader);

    CShaderProgram *pParticleShader = new CShaderProgram;
    pParticleShader->CreateProgram();
    pParticleShader->AddShaderToProgram(&shShaders[10]);
    pParticleShader->AddShaderToProgram(&shShaders[11]);
    pParticleShader->LinkProgram();
    m_pShaderPrograms->push_back(pParticleShader);

	// Create the skybox
	// Skybox downloaded from https://opengameart.org/content/space-skybox-1
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	//m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
    m_pFtFont->LoadFont("resources\\fonts\\LazenbyCompLiquid.ttf", 30); //Font downloaded from :https://www.dafont.com/lazenby-computer.font on 04/03/2020 (CC BY-SA 4.0)
	m_pFtFont->SetShaderProgram(pFontProgram);
    

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
    m_pPlayer->SetAudio(m_pAudio);
    //Create the squarePyramid
    m_pSquarePyramid->Create("resources\\textures\\", "pyramid.jpg", 11.0f, 11.0f, 10.0f, 1.0f); //Texture by me

    m_pPlanet->Load("resources\\models\\Planet\\Planet.obj"); //made using blender
    m_pIsocahedron->Create("resources\\textures\\", "ShieldGreen.png");

    //Create the GUI background
    m_pGUI->Create("resources\\textures\\", "pyramid.jpg", width* 0.6f, height*0.5f, 1.0f);

    //Creating and initialising collidables
    unsigned int numberCollidables = 15;
    //Order = Obstacle Static, Obstacle Dynamic, Powerup Shield, Powerup Boost
    for (unsigned int i = 0; i < numberCollidables; i++) {
        float sample = (m_pCatmullRom->GetTrackLength() / (numberCollidables * 1.25)) * (i + 1); //samples obstacles across tracks
        glm::vec3 position;
        glm::vec3 positionAhead;
        m_pCatmullRom->Sample(sample, position);
        m_pCatmullRom->Sample(sample + 2.0f, positionAhead);
        glm::vec3 T = glm::normalize(positionAhead - position);
        glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0,1,0)));
        glm::vec3 B = glm::normalize(glm::cross(N, T));

        //Random Generator http://www.cplusplus.com/reference/random/
        //https://stackoverflow.com/questions/34490599/c11-how-to-set-seed-using-random
        std::random_device r;
        std::default_random_engine generator{r()};
        std::uniform_real_distribution<float> distribution(-m_pCatmullRom->GetTrackWidth() / 2, m_pCatmullRom->GetTrackWidth() / 2);
        switch (i%4) {
            case 1:
            {
                float scale = 4.0f;
                glm::vec3 objectPosition = position + (B * scale);
                float horizontal = distribution(generator);
                objectPosition += N * horizontal;
                CAsteroid *collidable = new CAsteroid;
                collidable->Initialise(objectPosition, scale, sample, "STATIC");
                collidable->SetPlayerOrientation(glm::mat4(glm::mat3(-T, B, -N)));
                collidable->GetTNBFrame().Set(T, N, B);
                m_collidableObjects->push_back(collidable);
               
            }
            break;
            case 0:
            {
                float scale = 4.0f;
                glm::vec3 objectPosition = position + (B * scale);
                CAsteroid *collidable = new CAsteroid;
                collidable->Initialise(objectPosition, scale, sample, "DYNAMIC");
                collidable->SetPlayerOrientation(glm::mat4(glm::mat3(-T, B, -N)));
                collidable->GetTNBFrame().Set(T, N, B);
                m_collidableObjects->push_back(collidable);

            }
            break;
            case 2:
            {
                float scale = 3.0f;
                glm::vec3 objectPosition = position + (B * scale);
                CPowerup *collidable = new CPowerup;
                collidable->Initialise(objectPosition, scale, sample, "BOOST");
                collidable->SetPlayerOrientation(glm::mat4(glm::mat3(-T, B, -N)));
                collidable->GetTNBFrame().Set(T, N, B);
                m_collidableObjects->push_back(collidable);

            }
            break;
            case 3:
            {
                float scale = 3.0f;
                glm::vec3 objectPosition = position + (B * scale);
                CPowerup *collidable = new CPowerup;
                collidable->Initialise(objectPosition, scale, sample, "SHIELD");
                collidable->SetPlayerOrientation(glm::mat4(glm::mat3(-T, B, -N)));
                collidable->GetTNBFrame().Set(T, N, B);
                m_collidableObjects->push_back(collidable);

            }
            break;
        }
    }
    m_pAsteroid = new COpenAssetImportMesh;
    m_pAsteroid->Load("resources\\models\\Asteroid\\Asteroid1.obj");
    //create FBO
    m_pFrameBufferWindow->Create("resources\\textures\\", "pyramid.jpg", width, height, 1.0f); //uses random texture, won't be shown
    m_pFBO->Create(width, height);
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
    RECT dimensions = m_gameWindow.GetDimensions();
    float height = dimensions.bottom - dimensions.top;
    float width = dimensions.right - dimensions.left;

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

    //rendering with FBO
    m_pFBO->Bind();
    RenderScene(modelViewMatrixStack, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    modelViewMatrixStack.SetIdentity(); //Added before second pass
    RenderScene(modelViewMatrixStack, 1);

    CShaderProgram *pGUIShader = (*m_pShaderPrograms)[2];
    pGUIShader->UseProgram();

    //--------RENDERING HUD AND TEXT-----------
    //Render GUI background
    if (m_displayHUD) {
        //Displays backgroundplane that will act as hud for player 
        //uses orthograhic projection
        glDisable(GL_DEPTH_TEST);
        modelViewMatrixStack.Push();
            modelViewMatrixStack.SetIdentity();
            modelViewMatrixStack.Translate(glm::vec3(1.0f, 50.0f, 0.0f));
            modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(90.0f));
            pGUIShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
            pGUIShader->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
            pGUIShader->SetUniform("time", (float)m_gameTime);
            pGUIShader->SetUniform("guiWidth", m_pGUI->GetWidth());
            pGUIShader->SetUniform("guiHeight", m_pGUI->GetHeight());
            pGUIShader->SetUniform("timeBoosting", (float)m_pPlayer->GetTimeBoosting());
            pGUIShader->SetUniform("numberBoosts", (float)m_pPlayer->GetNumBoost());
            pGUIShader->SetUniform("maxBoost", (float)m_pPlayer->GetMaxBoost()); 
            m_pGUI->Render(false);
        modelViewMatrixStack.Pop();


        //displays text on hud showing speed, boost status, shields, current lap, current time.
        CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];
        glm::vec4 fontColour = glm::vec4(1.0f, 1.0f, 1.0f, 0.7f);
        fontProgram->UseProgram();
        fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
        fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
        fontProgram->SetUniform("vColour", fontColour);

        m_pFtFont->Print("Time: " + std::to_string((int)(m_gameTime * 0.001f)), 0.0125f*width, height * 0.26f, 20);
        m_pFtFont->Print("Lap: " + std::to_string(m_pCatmullRom->CurrentLap(m_currentDistance)) +"/" + std::to_string(m_maxLaps), 0.235f*width, height * 0.26f, 20);
        m_pFtFont->Print("Boost: ", width * 0.045f, height * 0.185f, 30);
        m_pFtFont->Print("Shields: ", width * 0.045f, height * 0.12f, 30);
        std::string shields = "";
        if (m_pPlayer->GetShield()) {
            fontProgram->SetUniform("vColour", glm::vec4(0.28, 0.73f, 0.26f, 0.7f));
            shields = "ACTIVE";
        } else {
            fontProgram->SetUniform("vColour", glm::vec4(0.84f, 0.02f, 0.0f, 0.7f));
            shields = "INACTIVE";
        }
        m_pFtFont->Print(shields, width * 0.2f, height * 0.12f, 30);
        fontProgram->SetUniform("vColour", fontColour);
        m_pFtFont->Print("Speed: ", width * 0.045f, height * 0.055f, 30);
        m_pFtFont->Print(std::to_string((int)(m_pPlayer->GetSpeed() * 100)*100) + "mph", width * 0.195f, height * 0.055f, 30);
    }

    CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];
    fontProgram->UseProgram();
    fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
    fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
    //Game over textt, 1 == game over from failure, 2 = game over from success
    if (m_gameOver == 1 || m_gameOver == 2) {
        if (m_gameOver == 1) {
            glm::vec4 fontColour = glm::vec4(0.72f, 0.07f, 0.07f, 1.0f); //red
            fontProgram->SetUniform("vColour", fontColour);
            m_pFtFont->Print("GAME OVER", width * 0.36, height * 0.445, 100);
        } else if (m_gameOver == 2) {
            glm::vec4 fontColour = glm::vec4(0.08f, 0.73f, 0.18f, 1.0f); //green
            fontProgram->SetUniform("vColour", fontColour);
            m_pFtFont->Print("COMPLETE", width * 0.37, height * 0.445, 100);
            m_pFtFont->Print("TIME: " + std::to_string((int)(m_gameTime * 0.001f)), width * 0.37, height * 0.3, 80);
        }
    }
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}
void Game::RenderScene(glutil::MatrixStack &modelViewMatrixStack, int pass) {

    // Clear the buffers and enable depth testing (z-buffering)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glm::vec4 lightPosition1 = glm::vec4(950, -122, -1600, 1); // Position of light source *in world coordinates*

    // Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
    // Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
    modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
    glm::mat4 viewMatrix = modelViewMatrixStack.Top();
    glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

    //Render instanced
    CShaderProgram *pInstancedShader = (*m_pShaderPrograms)[4];
    pInstancedShader->UseProgram();
    pInstancedShader->SetUniform("bUseTexture", true);
    pInstancedShader->SetUniform("time", (float)m_gameTime);
    pInstancedShader->SetUniform("sampler0", 0);
    pInstancedShader->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
    pInstancedShader->SetUniform("light1.La", glm::vec3(0.3f));		// Ambient colour of light
    pInstancedShader->SetUniform("light1.Ld", glm::vec3(1.5f));		// Diffuse colour of light
    pInstancedShader->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
    pInstancedShader->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
    pInstancedShader->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
    pInstancedShader->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
    pInstancedShader->SetUniform("material1.shininess", 15.0f);		// Shininess material property

    int numberOfAsteroids = 3000;
    pInstancedShader->SetUniform("numberOfAsteroids", numberOfAsteroids);
    modelViewMatrixStack.Push();
        modelViewMatrixStack.Translate(glm::vec3(0, 0, 0));
        pInstancedShader->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
        pInstancedShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pInstancedShader->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pAsteroid->RenderInstanced(numberOfAsteroids);
    modelViewMatrixStack.Pop();

    // Use the main shader program 
    CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
    pMainProgram->UseProgram();
    pMainProgram->SetUniform("bUseTexture", true);
    pMainProgram->SetUniform("sampler0", 0);
    pMainProgram->SetUniform("CubeMapTex", 1);


    // Set the projection matrix
    pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
    
    // Set light and materials in main shader program
    pMainProgram->SetUniform("numberOfPowerups", 2);
    pMainProgram->SetUniform("lights[0].position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
    pMainProgram->SetUniform("lights[0].Ld", glm::vec3(1.5f));		// Diffuse colour of light
    pMainProgram->SetUniform("lights[0].Ls", glm::vec3(1.0f));		// Specular colour of light
    pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
    pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
    pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
    pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

    pMainProgram->SetUniform("time", (float)m_gameTime);		// Game time in milliseconds
    pMainProgram->SetUniform("lights[1].position", viewMatrix*glm::vec4(m_pPlayer->GetPosition() + (m_pPlayer->GetUpVector() * -0.2f), 1));// Position of light at playerPosition
    pMainProgram->SetUniform("lights[1].La", glm::vec3(0.3f));		// Ambient colour of light

    //if boosting alter the ambient colour to be more blue
    if (m_pPlayer->GetBoost() > 1.0f) {
        pMainProgram->SetUniform("lights[1].La", glm::vec3(0.24f, 0.89f, 0.75f) * m_pPlayer->GetBoost());
    } else {
        pMainProgram->SetUniform("lights[1].La", glm::vec3(0.3f));
    }

    if ((int)(m_gameTime / 1000) % 3 == 0) {
        pMainProgram->SetUniform("lights[1].Ld", glm::vec3(3.0f, 0.0f, 0.0f));
        pMainProgram->SetUniform("lights[1].Ls", glm::vec3(3.0f, 0.0f, 0.0f));
    } else if ((int)(m_gameTime / 1000) % 2 == 0) {
        pMainProgram->SetUniform("lights[1].Ld", glm::vec3(0.0f, 3.0f, 0.0f));
        pMainProgram->SetUniform("lights[1].Ls", glm::vec3(0.0f, 3.0f, 0.0f));
    } else if ((int)(m_gameTime / 1000) % 1 == 0) {
        pMainProgram->SetUniform("lights[1].Ld", glm::vec3(0.0f, 0.0f, 3.0f));
        pMainProgram->SetUniform("lights[1].Ls", glm::vec3(0.0f, 0.0f, 3.0f));
    }
    pMainProgram->SetUniform("lights[1].direction", glm::normalize(viewNormalMatrix*glm::vec3(m_pPlayer->GetUpVector() * -1.0f * 5.0f)));// Direction of light
    pMainProgram->SetUniform("lights[1].exponent", 10.0f);
    pMainProgram->SetUniform("lights[1].cutoff", 55.0f);


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

    // Turn on diffuse + specular materials
    pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
    pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
    pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

    // Render the square pyramid
    modelViewMatrixStack.Push();
        modelViewMatrixStack.Translate(glm::vec3(600.0f, -360.0f + sin(m_gameTime / 1000)*30.0f, -1780.0f));
        modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), (m_gameTime / 1000)*0.25f);
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
    
    //Render the collidables
    if (m_collidableObjects != NULL) {
        for (unsigned int i = 0; i < m_collidableObjects->size(); i++) {
            modelViewMatrixStack.Push();
                pMainProgram->SetUniform("bUseTexture", true);
                modelViewMatrixStack.Translate((*m_collidableObjects)[i]->GetPosition());
                modelViewMatrixStack *= (*m_collidableObjects)[i]->GetPlayerOrientation();
                //constantly rotates
                glm::vec3 rotationalVector = glm::vec3(0, 0, 0);
                if ((*m_collidableObjects)[i]->GetType() == "ASTEROID") {
                    pMainProgram->SetUniform("pulse", false);
                    rotationalVector = (*m_collidableObjects)[i]->GetTNBFrame().T;
                } else if ((*m_collidableObjects)[i]->GetType() == "POWERUP") {
                    pMainProgram->SetUniform("pulse", true);
                    rotationalVector = (*m_collidableObjects)[i]->GetTNBFrame().B;
                }
                modelViewMatrixStack.Rotate(rotationalVector, (m_gameTime / 1000));
                modelViewMatrixStack.Scale((*m_collidableObjects)[i]->GetRadius());
                pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
                pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
                (*m_collidableObjects)[i]->Render();
                pMainProgram->SetUniform("pulse", false);
            modelViewMatrixStack.Pop();
        }
    }
    
    //render player
    modelViewMatrixStack.Push();
        pMainProgram->SetUniform("bUseTexture", true);
        m_pPlayer->GetPosition().y += sin(m_gameTime / 250)*m_pPlayer->GetSpeed() * 0.3f;
        modelViewMatrixStack.Translate(m_pPlayer->GetPosition());
        modelViewMatrixStack *= m_pPlayer->GetPlayerOrientation();
        modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), -m_pPlayer->GetSideSpeed() * 0.4f);
        modelViewMatrixStack.Scale(m_pPlayer->GetScale());
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pPlayer->Render();
    modelViewMatrixStack.Pop();

    //Enabling transparent texture
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //render track
    modelViewMatrixStack.Push();
        pMainProgram->SetUniform("bUseTexture", true);
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pCatmullRom->RenderTrack();
    modelViewMatrixStack.Pop();


    //render isocahedron
    modelViewMatrixStack.Push();
        pMainProgram->SetUniform("bUseTexture", true);
        modelViewMatrixStack.Translate(glm::vec3(440, -390, -1550));
        modelViewMatrixStack.Scale(10.0f);
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pIsocahedron->Render();
    modelViewMatrixStack.Pop();

    //Render player Shield
    modelViewMatrixStack.Push();
        pMainProgram->SetUniform("bUseTexture", true);
        modelViewMatrixStack.Translate(m_pPlayer->GetPosition());
        modelViewMatrixStack *= m_pPlayer->GetPlayerOrientation();
        modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), -m_pPlayer->GetSideSpeed() * 0.4f);
        modelViewMatrixStack.Scale(m_pPlayer->GetScale() * 4.0f);
        pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
        pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pPlayer->RenderShield();
    modelViewMatrixStack.Pop();
    

    
    //POST PROCESSING
    if (pass == 1) {
        CShaderProgram *pPostProcessingShader = (*m_pShaderPrograms)[3];
        pPostProcessingShader->UseProgram();
        pPostProcessingShader->SetUniform("sampler0", 0);
        pPostProcessingShader->SetUniform("width", (int)(m_pFBO->GetWidth()));
        pPostProcessingShader->SetUniform("height", (int)(m_pFBO->GetHeight()));
        pPostProcessingShader->SetUniform("isBoosting", m_pPlayer->GetBoost() > 1.0f);

        // Render the plane for the TV
        glDisable(GL_CULL_FACE);
        modelViewMatrixStack.Push();
            modelViewMatrixStack.SetIdentity();
            modelViewMatrixStack.Translate(glm::vec3(m_pFBO->GetWidth()*0.5f, m_pFBO->GetHeight()*0.5f, 0.0f));
            modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f));
            //modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f));
            //modelViewMatrixStack.Scale(-1.0f);
            pPostProcessingShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
            pPostProcessingShader->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
            // To turn off texture mapping and use the plane colour only (currently white material), uncomment the next line
            m_pFBO->BindTexture(0);
            m_pFrameBufferWindow->Render(false);
        modelViewMatrixStack.Pop();
        glEnable(GL_CULL_FACE);
    }
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

    //Perspective changes with faster speed
    float perspective = m_pPlayer->GetSpeed() < 0 ? 0 : m_pPlayer->GetSpeed() * 1.0f;
    perspective = perspective > 1.0f + ((m_pPlayer->GetBoost()-1) * 0.18f) ? 1.0f + ((m_pPlayer->GetBoost() - 1) * 0.18f) : perspective;
    m_pCamera->SetPerspectiveProjectionMatrix(45.0f + perspective, (float)width / (float)height, 0.5f, 5000.0f);

    m_pPlayer->Update(m_dt, m_pCatmullRom->GetTrackWidth() * 0.5f, playerUpdate);
    m_currentDistance += m_pPlayer->GetSpeed() * (float)m_dt * 0.1f;
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
    m_pPlayer->BoostAcceleration(m_dt);
    CheckCollisions();
    UpdateCollidables();
    CheckGameOver();
	m_pAudio->Update();
}

//Checks if the player has collided with an object
//For efficiency, a number is kept which knows which obstacle is next on the track which will be used to check collisions isntead of going through every item.
void Game::CheckCollisions() {
    bool checked = false;
    if ((*m_collidableObjects)[m_currentCheck % m_collidableObjects->size()]->CheckCollision(m_pPlayer->GetPosition(),m_pPlayer->GetScale())) {
        if ((*m_collidableObjects)[m_currentCheck % m_collidableObjects->size()]->GetType() == "ASTEROID") {
            if (m_pPlayer->GetShield()) {
                m_pPlayer->DeactivateShield();
                m_pAudio->LoadEventSound("Resources\\Audio\\shieldBreak.wav"); //https://freesound.org/people/aust_paul/sounds/30937/ by “aust_paul” on 09/03/2020 (CC0 1.0 Universal (CC0 1.0) Public Domain Dedication)
                m_pAudio->PlayEventSound();
            } else {
                m_pPlayer->TakeDamage();
                m_pAudio->LoadEventSound("Resources\\Audio\\shipDestruction.mp3"); //https://freesound.org/people/deleted_user_1941307/sounds/155790/ by “deleted_user_1941307” on 09/03/2020 (CC0 1.0 Universal (CC0 1.0) Public Domain Dedication)
                m_pAudio->PlayEventSound();
            }
        } else if ((*m_collidableObjects)[m_currentCheck % m_collidableObjects->size()]->GetType() == "POWERUP") {
            CPowerup* currentPowerup = dynamic_cast<CPowerup*>((*m_collidableObjects)[m_currentCheck % m_collidableObjects->size()]);
            if (currentPowerup->GetPowerupType() == "BOOST") {
                m_pPlayer->IncrementBoost();
                m_pAudio->LoadEventSound("Resources\\Audio\\boostPowerup.wav"); //https://freesound.org/people/Bird_man/sounds/316743/ by “Bird_man” on 09/03/2020 (CC0 1.0 Universal (CC0 1.0) Public Domain Dedication)
                m_pAudio->PlayEventSound();
            } else if ((currentPowerup->GetPowerupType() == "SHIELD")) {
                m_pPlayer->ActivateShield();
                m_pAudio->LoadEventSound("Resources\\Audio\\shieldPowerup.wav"); //https://freesound.org/people/Eschwabe3/sounds/460133/ by “Eschwabe3” on 09/03/2020 (Attribution 3.0 Unported (CC BY 3.0))
                m_pAudio->PlayEventSound();
            }
        }
        m_currentCheck++;
        checked = true;
    }
    //if there was no collisions, check to see if the player has gone passed the object position, if yes increment the object counter
    //You have to use the distance depending on the lap as the distance doesn't reset to zero
    if (!checked) {
        float currentDistanceNormalised = m_currentDistance - m_pCatmullRom->CurrentLap(m_currentDistance)*m_pCatmullRom->GetTrackLength();
        if (currentDistanceNormalised >= (*m_collidableObjects)[m_currentCheck % m_collidableObjects->size()]->GetPlacementOnTrack()) {
            if(currentDistanceNormalised <= (*m_collidableObjects)[m_collidableObjects->size()-1]->GetPlacementOnTrack()){
                m_currentCheck++;
            } else {
                m_currentCheck = 0;
            }
        }
    }
}

void Game::CheckGameOver() {
    if (m_pPlayer->GetHealth() == 0) {
        m_paused = true;
        m_gameOver = 1;
    }
    if (m_pCatmullRom->CurrentLap(m_currentDistance) == m_maxLaps) {
        m_paused = true;
        m_gameOver = 2;
    }
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
        //m_pFtFont->Print("Player: " + glm::to_string(m_pPlayer->GetPosition()), 20, height - 20, 20);
        //m_pFtFont->Print("Asteroid: " + glm::to_string((*m_collidableObjects)[0]->GetPosition()),20, height-50,20);
        m_pFtFont->Print("Camera: " + glm::to_string(m_pCamera->GetPosition()), 20, height - 80, 20);
        //m_pFtFont->Print("Current Check: " +std::to_string(m_currentCheck), 20, height - 110, 20);
        //m_pFtFont->Print("Time Boosting: " + std::to_string((float)m_pPlayer->GetTimeBoosting()), 20, height - 140, 20);
        //m_pFtFont->Print("Health: " + std::to_string(m_pPlayer->GetHealth()), 20, height - 170, 20);
        //m_pFtFont->Print("Game time: " + std::to_string(m_gameTime), 20, height - 200, 20);
        //m_pFtFont->Print(std::to_string(m_pPlayer->GetSpeed()), 20, height - 20, 20);
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
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

void Game::UpdateCollidables() {
    if (m_collidableObjects != NULL) {
        for (unsigned int i = 0; i < m_collidableObjects->size(); i++) {
            if ((*m_collidableObjects)[i]->GetType() == "ASTEROID") {
                CAsteroid* currentAsteroid = dynamic_cast<CAsteroid*>((*m_collidableObjects)[i]);
                if (currentAsteroid->GetAsteroidType() == "DYNAMIC") {
                    //This method only works if using track centre then adding sin value
                    int multiplier = (i * 100) < 500 ? (i * 100) : 500; //double speed of base speed is max
                    glm::vec3 newPosition = ((float)(sin(m_gameTime / (1000-multiplier)))- (float)(sin(currentAsteroid->GetPreviousGameTime() / (1000-multiplier)))) * currentAsteroid->GetTNBFrame().N * m_pCatmullRom->GetTrackWidth() * 0.5f;

                    currentAsteroid->SetPosition(newPosition+currentAsteroid->GetPosition());
                    currentAsteroid->SetPreviousGameTime(m_gameTime);
                }
            } else if ((*m_collidableObjects)[i]->GetType() == "POWERUP") {
                CPowerup* currentPowerup = dynamic_cast<CPowerup*>((*m_collidableObjects)[i]);
                //This method only works if using track centre then adding sin value
                int multiplier = (i * 100) < 500 ? (i * 100) : 500; //double speed of base speed is max
                glm::vec3 newPosition = ((float)(sin(m_gameTime / (1000 - multiplier))) - (float)(sin(currentPowerup->GetPreviousGameTime() / (1000 - multiplier)))) * currentPowerup->GetTNBFrame().N * m_pCatmullRom->GetTrackWidth() * 0.5f;

                currentPowerup->SetPosition(newPosition + currentPowerup->GetPosition());
                currentPowerup->SetPreviousGameTime(m_gameTime);
                
            }
        }
    }
}

void Game::Reset() {
    m_paused = false;
    m_currentDistance = 0;
    m_currentCheck = 0;
    m_pPlayer->Reset();
    m_gameTime = 0;
    m_gameOver = false;
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
            if (!m_paused) {
                GameLoop();
            }
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
			    break;
            //added case to cycle camera view type
            case 0x50: //P key
                m_pCamera->CycleViewType();
                break;
            case 0x4C: //L key
                {
                    m_pCamera->SetViewType(2);
                }
                break;
            case 0x20: //space key
                {
                    m_paused = m_paused == true? false: true;
                }
                break;
            case 0x52: //R key
                {
                    Reset();
                }
                break;
            case 0x4D: //M key
                {
                    m_displayHUD = m_displayHUD == true ? false : true;
                }
                break;
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
