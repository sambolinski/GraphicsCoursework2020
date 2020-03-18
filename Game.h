#pragma once

#include "Common.h"
#include "GameWindow.h"
#include "MatrixStack.h"
// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CPlayer;
class CCube;
class CSquarePyramid;
class CCollidable;
class CIsocahedron;
class CFrameBufferObject;
class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pGUI;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pCatmullRom;
    CPlayer *m_pPlayer;
    CCube *m_pCube;
    CSquarePyramid *m_pSquarePyramid;
    COpenAssetImportMesh *m_pPlanet;
    CIsocahedron *m_pIsocahedron;
	// Some other member variables
    vector<CCollidable *> *m_collidableObjects;
    CFrameBufferObject *m_pFBO;
    CPlane *m_pFrameBufferWindow;
    vector<COpenAssetImportMesh *> *m_asteroids;
    float m_currentDistance;
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
    int m_currentCheck = 0;
    void CheckCollisions();
	void DisplayFrameRate();
    void DisplayMessage(std::string &message, float x, float y, float size);
	void GameLoop();
    void UpdateCollidables();
    void CheckGameOver();
    void Reset();
    void RenderScene(glutil::MatrixStack &modelViewMatrixStack, int pass);
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
    double m_gameTime;
    int m_gameOver = 0;
    bool m_paused = false;
    bool m_displayHUD = true;
    unsigned int m_maxLaps;

};
