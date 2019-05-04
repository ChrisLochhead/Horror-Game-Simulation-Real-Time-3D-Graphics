#pragma once
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#define DEG_TO_RADIAN 0.017453293

#include "Cube.h"
#include "Light.h"
#include "rt3d.h"
#include "Material.h"
#include "Camera.h"
#include "SkyBox.h"
#include "Player.h"
#include "Bed.h"
#include "Coin.h"
#include "Audio.h"
#include "Enemy.h"
#include "CoinCollider.h"
#include "EnemyCollider.h"
#include "WallCollider.h"
#include "DoorCollider.h"
#include <SDL_ttf.h>
#include <SDL.h>
#include <string>
#include <sstream>
#include <string.h>

using namespace std;

class Game {
public:
	Game()
	{
		eye = { 5.0f, 0.5f, 0.75f };

	}

	GLuint loadBitmap(char *fname, bool skybox); // loading textures and skyboxes

	void drawSkyBox(); // draws the skybox

	void renderMap(); // controls drawing of the game environment

	void renderGarden(); // sub-function of render map
	void renderHouse(); // sub-function of render map
	void drawCoins(); // sub-function of render map
	void buildColliders(); // sub-function of render map

	SDL_Window* Window(SDL_GLContext &context); // creates a game window
	SDL_Window* getWindow() { return hWindow; } 

	void render(); // draw function

	void update(); // update function

	void init(void); // game initialisation

	virtual ~Game()
	{
		if (light != nullptr) { delete light; }
		if (material != nullptr) { delete material; }
		if (materialDark != nullptr) { delete materialDark; }
		if (materialDirt != nullptr) { delete materialDirt; }
		if (cube != nullptr) { delete cube; }
		if (door != nullptr) { delete door; }
		if (bed != nullptr) { delete bed; }
		if (coin != nullptr) { delete coin; }
		if (player != nullptr) { delete player; }
		if (skybox != nullptr) { delete skybox; }
		if (camera != nullptr) { delete camera; }


		for (vector<CollisionEntity*>::iterator it = collisionObjs.begin(); it < collisionObjs.end(); it++)
		{
			delete *it;
		}

	} // destructor function

	GLuint textToTexture(const char * str, GLuint textID, Uint8 r, Uint8 g, Uint8 b);// draws text to screen for HUD

	void playSound(Audio* sound); // plays sounds

	bool checkCollision(CollisionEntity* cube1, CollisionEntity* cube2, int position1, int position2); // cycles through collision entities and tests for collision

	void hideCoin(int position); // hides coin from view and disables collision

	

private:

	glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	int scaleSetter = 0;
	int direction = 0; 

	DoorCollider* door;
	WallCollider* wall[30];
	CoinCollider* collider[3]; 
	EnemyCollider* enemyCollider;// collision stuff

	md2model model;

	GLuint mVertCount = 0;
	GLfloat r = 0.0f;

	GLuint textures[20];

	glm::vec3 eye;        // for setting up lookat function
	glm::vec3 at{ 10.0f, 1.0f, 3.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };

	GLuint mvpShaderProgram; // general shader
	GLuint skyBoxShaderProgram; // shader specifically for skyboxes

	glm::mat4 projection = glm::mat4(1.0);
	glm::mat4 modelview = glm::mat4(1.0);  // projection and modelview matrices

	Light* light = nullptr;
	Material* material = nullptr;
	Material* materialDark = nullptr;
	Material* materialDirt = nullptr;
	Material* materialTransparent = nullptr;
	Cube* cube = nullptr;
	Bed* bed = nullptr;
	Coin* coin[3];
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	SkyBox* skybox = nullptr;
	Camera* camera = nullptr;
	SDL_Window * hWindow = nullptr; // all objects utilised by the game 

	std::vector<CollisionEntity*> collisionObjs; // for detecting collision
	bool gameExit = NULL;
	bool gameWon = NULL;
	bool gameLost = NULL;
	GLuint meshObjects[10];

	glm::vec3 eyeVector;
	GLfloat forwardMove = 0.0f;
	GLfloat rightMove = 0.0f;
	GLfloat xTurn = 0.0f;  // for moving the player
	GLfloat yTurn = 0.0f;

	GLfloat enemyForwardMove = 0.0f;
	GLfloat enemyRightMove = 0.0f;

	GLfloat coinAngle = 0.0f; // for letting the coins rotate ambiently

	Audio* samples[10];  
	bool playing = false;   // audio stuff

	TTF_Font* font;
	GLuint labels[10];
	int Score;// HUD stuff

	Cube* testCube;
};