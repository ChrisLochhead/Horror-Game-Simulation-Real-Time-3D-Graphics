#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include "md2model.h"
#include "Camera.h"
#include "Game.h"
#include <SDL_ttf.h>

using namespace std;

SDL_Window * Game::Window(SDL_GLContext &context) {
	//SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

	// Create 800x600 window
	hWindow = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!hWindow) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(hWindow); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return hWindow;
}
GLuint Game::loadBitmap(char *fname, bool istrue) {
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

	// load file - using core SDL library
	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface) {
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	//if (istrue == true) {
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//}
	//else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//}

	SDL_PixelFormat *format = tmpSurface->format;

	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}
void Game::init(void) {

	Score = 0;
	for (int i = 0; i < 6; i++) {
		labels[i] = 0;
	}

	if (TTF_Init() == -1)
		cout << "TTF failed to initialise." << endl;

	font = TTF_OpenFont("Res/Fonts/Bold Font.ttf", 150);
	if (font == NULL)
		cout << "Failed to open font." << endl;

	skyBoxShaderProgram = rt3d::initShaders("Res/Shaders/NoLight.vert", "Res/Shaders/NoLight.frag");
	mvpShaderProgram = rt3d::initShaders("Res/Shaders/phong-tex.vert", "Res/Shaders/phong-tex.frag");


	cube = new Cube();
	testCube = new Cube();

	enemyCollider = new EnemyCollider();
	door = new DoorCollider();

	for (int i = 0; i < 3; i++)
	{
		collider[i] = new CoinCollider();
	}
	for (int i = 0; i < 30; i++)
	{
		wall[i] = new WallCollider();
	}

	camera = new Camera();
	player = new Player();
	enemy = new Enemy();

	coin[0] = new Coin();
	coin[1] = new Coin();
	coin[2] = new Coin();

	material = new Material(1);
	materialDark = new Material(2);
	materialDirt = new Material(3);
	materialTransparent = new Material(4);

	skybox = new SkyBox();

	if (!BASS_Init(-1, 44100, 0, 0, NULL))
		cout << "Can't initialize device";


	samples[0] = new Audio("Res/Audio/Background music.wav");
	samples[1] = new Audio("Res/Audio/Footsteps(1).wav");

	samples[0]->playSound(0.1);

	textures[0] = loadBitmap("Res/Objects/bed.bmp", false);
	textures[1] = loadBitmap("Res/Objects/hayden.bmp", false);
	textures[2] = loadBitmap("Res/Objects/DampWoodIndividual.bmp", false);
	textures[3] = loadBitmap("Res/Objects/WoodenFloor.bmp", false);
	textures[4] = loadBitmap("Res/Objects/BrickWall.bmp", false);
	textures[5] = loadBitmap("Res/Objects/Grass.bmp", false);
	textures[6] = loadBitmap("Res/SkyBox/mnight_ft.bmp", true);
	textures[7] = loadBitmap("Res/SkyBox/mnight_bk.bmp", true);
	textures[8] = loadBitmap("Res/SkyBox/mnight_dn.bmp", true);
	textures[9] = loadBitmap("Res/SkyBox/mnight_up.bmp", true);
	textures[10] = loadBitmap("Res/SkyBox/mnight_lf.bmp", true);
	textures[11] = loadBitmap("Res/SkyBox/mnight_rt.bmp", true);
	textures[12] = loadBitmap("Res/Objects/DirtRoad.bmp", true);
	textures[13] = loadBitmap("Res/Objects/300coin.bmp", false);
	textures[14] = loadBitmap("Res/Objects/Door.bmp", false);
	textures[15] = loadBitmap("Res/Objects/hueteotl.bmp", false);

	meshObjects[0] = skybox->getMesh();
	meshObjects[2] = model.ReadMD2Model("Res/Objects/BED.md2");
	meshObjects[3] = cube->getMesh();
	meshObjects[4] = coin[0]->getMesh();
	mVertCount = model.getVertDataCount();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(mvpShaderProgram);
	light = new Light(1);

	rt3d::setLight(mvpShaderProgram, light->getLight());
	rt3d::setMaterial(mvpShaderProgram, material->getMaterial());

	glm::vec4 tmp = camera->getTop()*light->getLightPos();
	rt3d::setLightPos(mvpShaderProgram, glm::value_ptr(tmp));

	camera->pushModelview();

	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 1400.0f / 800.0f, 1.0f, 500.0f);
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "projection", glm::value_ptr(projection));

	glm::mat4 modelview(1.0); // set base position for scene
	camera->pushModelview();
	tmp = camera->getTop()*light->getLightPos();

	float r_new = r - 180;
	glm::vec3 vec_offset = { std::cos(r_new), 1.5f, -std::sin(r_new) };
	at = { playerPosition.z, playerPosition.y + 2.0, playerPosition.x };
	eye = at + vec_offset;
	camera->setTop(glm::lookAt(eye, at, up));
	drawSkyBox();

	glDepthMask(GL_TRUE); // make sure depth test is on
	glUseProgram(mvpShaderProgram);

	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 1400.0f / 800.0f, 1.0f, 500.0f);
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "projection", glm::value_ptr(projection));

	rt3d::setLightPos(mvpShaderProgram, glm::value_ptr(tmp));
	rt3d::setMaterial(mvpShaderProgram, materialDark->getMaterial());
	glBindTexture(GL_TEXTURE_2D, textures[12]);
	camera->pop();
	renderMap();


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d * std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d * std::cos(angle*DEG_TO_RADIAN));
}
glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d * std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d * std::sin(angle*DEG_TO_RADIAN));
}
void Game::playSound(Audio* sound)
{
	if (playing == false) {
		sound->playSound(0.5);
		playing = true;
	}
}
GLuint Game::textToTexture(const char * str, GLuint textID, Uint8 r, Uint8 g, Uint8 b) {
	GLuint texture = textID;
	TTF_Font * Font = font;

	SDL_Surface * stringImage = TTF_RenderText_Blended(Font, str, { r, g, b });

	if (stringImage == NULL) {
		std::cout << "String surface not created." << std::endl;
	}

	if (texture == 0) {
		glGenTextures(1, &texture);//This avoids memory leakage, only initialise //first time
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stringImage->w, stringImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, stringImage->pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);

	SDL_FreeSurface(stringImage);

	return texture;
}

void Game::drawItem(glm::vec3 pos, glm::vec3 scale, float rot, glm::vec3 rotDegree) {
	camera->pushBack(camera->getTop());
	camera->setTop(glm::scale(camera->getTop(), scale));
	camera->setTop(glm::rotate(camera->getTop(), rot, rotDegree));
	camera->setTop(glm::translate(camera->getTop(), pos));
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	cube->draw();
	camera->pop();
}

void Game::update(void) {
	//if the game isnt over
	if (gameExit != true &&
		gameLost != true) {

		const	Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) {

			//turn forward
			at = moveForward(at, r + yTurn, 0.1f), forwardMove += 0.1;
			player->setCurrentAnim(1);
			playSound(samples[1]);
			direction = 1;
			xTurn = 0.0f;

		}
		else if (keys[SDL_SCANCODE_S]) {

			//xTurn back
			at = moveForward(at, r + yTurn, -0.1f), forwardMove -= 0.1;
			xTurn = -180.0f;
			player->setCurrentAnim(1);
			playSound(samples[1]);
			direction = 2;

		}
		else if (keys[SDL_SCANCODE_A]) {

			//xTurn left
			at = moveRight(at, r + yTurn, -0.1f), rightMove -= 0.1;
			xTurn = -90.0f;
			player->setCurrentAnim(1);
			playSound(samples[1]);
			direction = 3;


		}
		else if (keys[SDL_SCANCODE_D]) {

			//xTurn right
			at = moveRight(at, r + yTurn, 0.1f); rightMove += 0.1;
			xTurn = 90.0f;
			player->setCurrentAnim(1);
			playSound(samples[1]);
			direction = 4;



		}
		else {

			//set conditions if stationary
			playing = false;
			samples[1]->playSound(0.0);
			player->setCurrentAnim(0);


		}
		if (keys[SDL_SCANCODE_R]) eye.y += 0.1;
		if (keys[SDL_SCANCODE_F]) eye.y -= 0.1;

		if (keys[SDL_SCANCODE_COMMA]) r += 0.1f, xTurn += 2.0f;
		if (keys[SDL_SCANCODE_PERIOD]) r -= 0.1f, xTurn -= 2.0f;
		if (keys[SDL_SCANCODE_K])  yTurn += 0.2f;
		if (keys[SDL_SCANCODE_L])  yTurn -= 0.2f;
	}
	else {
		player->setCurrentAnim(0);
	}

}

void Game::drawSkyBox(glm::vec3 pos, glm::vec3 scale, int texture, float rot, glm::vec3 rotDegree)
{
	camera->pushBack(camera->getTop());
	glBindTexture(GL_TEXTURE_2D, textures[texture]);
	camera->setTop(glm::scale(camera->getTop(), scale));
	camera->setTop(glm::translate(camera->getTop(), pos));
	camera->setTop(glm::rotate(camera->getTop(), float(rot * DEG_TO_RADIAN), rotDegree));
	rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	rt3d::drawIndexedMesh(meshObjects[0], skybox->getIndexCount(), GL_TRIANGLES);
	camera->pop();
}

void Game::drawSkyBox()
{
	glUseProgram(skyBoxShaderProgram);
	rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "projection", glm::value_ptr(projection));

	glDepthMask(GL_FALSE); // make sure depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(camera->getTop());
	camera->pushBack(glm::mat4(mvRotOnlyMat3));

	// front 
	drawSkyBox(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(50.0f, 50.0f, 50.0f), 6);
	drawSkyBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f, 50.0f, 50.0f), 7);
	drawSkyBox(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(50.0f, 50.0f, 50.0f), 8, 90, glm::vec3(1.0f, 0.0f, 0.0f));
	drawSkyBox(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(50.0f, 50.0f, 50.0f), 9, 90, glm::vec3(1.0f, 0.0f, 0.0f));
	drawSkyBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f, 50.0f, 50.0f), 10, 90, glm::vec3(0.0f, 1.0f, 0.0f));
	drawSkyBox(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(50.0f, 50.0f, 50.0f), 11, 90, glm::vec3(0.0f, 1.0f, 0.0f));

}
void Game::drawCoins() {
	//draw coins
	coinAngle += 0.5;
	camera->pushBack(camera->getTop());
	camera->setTop(glm::translate(camera->getTop(), glm::vec3(2.0f, 0.75f, -15.0)));
	camera->setTop(glm::rotate(camera->getTop(), float(coinAngle*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
	camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.0025f, 0.0025f, 0.0025f)));
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	coin[0]->draw();
	camera->pop();

	//coin collider
	rt3d::setMaterial(mvpShaderProgram, materialTransparent->getMaterial());
	camera->pushBack(camera->getTop());
	camera->setTop(glm::translate(camera->getTop(), glm::vec3(2.0f, 0.75f, -15.0f)));
	camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.3, 1.0, 0.3)));
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	//collider[0]->draw();
	collider[0]->setTranslate(glm::vec3(15.0f, 0.8f, -2.0f));
	collider[0]->setScale(glm::vec3(0.3, 1.0, 0.3), scaleSetter);
	collisionObjs.push_back(collider[0]);
	camera->pop();

	//coin 2
	camera->pushBack(camera->getTop());
	camera->setTop(glm::translate(camera->getTop(), glm::vec3(9.0f, 0.75f, -15.0)));
	camera->setTop(glm::rotate(camera->getTop(), float(coinAngle*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
	camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.0025f, 0.0025f, 0.0025f)));
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	coin[1]->draw();
	camera->pop();

	//coin collider
	rt3d::setMaterial(mvpShaderProgram, materialTransparent->getMaterial());
	camera->pushBack(camera->getTop());
	camera->setTop(glm::translate(camera->getTop(), glm::vec3(9.0f, 0.75f, -15.0f)));
	camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.3, 1.0, 0.3)));
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	//collider[1]->draw();
	collider[1]->setTranslate(glm::vec3(15.0f, 0.8f, -9.0f));
	collider[1]->setScale(glm::vec3(0.3, 1.0, 0.3), scaleSetter);
	collisionObjs.push_back(collider[1]);
	camera->pop();

	//coin 3
	camera->pushBack(camera->getTop());
	camera->setTop(glm::translate(camera->getTop(), glm::vec3(9.0f, 0.75f, -21.0)));
	camera->setTop(glm::rotate(camera->getTop(), float(coinAngle*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
	camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.0025f, 0.0025f, 0.0025f)));
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	coin[2]->draw();
	camera->pop();

	//coin collider
	rt3d::setMaterial(mvpShaderProgram, materialTransparent->getMaterial());
	camera->pushBack(camera->getTop());
	camera->setTop(glm::translate(camera->getTop(), glm::vec3(9.0f, 0.75f, -21.0f)));
	camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.3, 1.0, 0.3)));
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
	//collider[2]->draw();
	collider[2]->setTranslate(glm::vec3(21.0f, 0.8f, -9.0f));
	collider[2]->setScale(glm::vec3(0.3, 1.0, 0.3), scaleSetter);
	collisionObjs.push_back(collider[2]);
	camera->pop();


}
void Game::renderMap()
{

//	drawCoins();
	renderGarden();
	renderHouse();
//	buildColliders();

}
void Game::renderGarden()
{
	//grass texture
	rt3d::setMaterial(mvpShaderProgram, materialDark->getMaterial());
	glBindTexture(GL_TEXTURE_2D, textures[5]); // grass texture

	//left hand path - front garden
	drawItem(glm::vec3(0.0, -10.0, 0.0), glm::vec3(4.0, 0.1, 7.0));
	//right hand path - front garden
	drawItem(glm::vec3(3.33, -10.0, 0.0), glm::vec3(3.0, 0.1, 7.0));
	// centre path
	glBindTexture(GL_TEXTURE_2D, textures[12]); // dirt texture
	drawItem(glm::vec3(3.0, -10.0, 0.0), glm::vec3(2.0, 0.1, 7.0));

	//back wall
	glBindTexture(GL_TEXTURE_2D, textures[4]);// brick wall texture
	drawItem(glm::vec3(5/9 + 0.55, -1.0/1.5, 7.0/0.3), glm::vec3(9.0, 1.5, 0.3));
	//left wall
	drawItem(glm::vec3(-4.0/0.3, -1.0/1.5, 0.0), glm::vec3(0.3, 1.5, 7.0));
	//right wall
	drawItem(glm::vec3(13.0/0.3, -1.0/1.5, 0.0), glm::vec3(0.3, 1.5, 7.0));

	//back garden
	glBindTexture(GL_TEXTURE_2D, textures[5]); // grass texture
	drawItem(glm::vec3(1.0, -1.0f/0.1, -50.0/20), glm::vec3(15.0f, 0.1f, 20.0f));
}

void Game::renderHouse()
{

	glBindTexture(GL_TEXTURE_2D, textures[14]); // door texture
		// back door, open and closed states
	if (Score == 3) {
		// closed
		drawItem(glm::vec3(4.5, 0.5, -29.5/0.1), glm::vec3(1.0, 1.0, 0.1));
	}
	else {
		//openstate
		//camera->pushBack(camera->getTop());
		//camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f))); // rotate to open
		drawItem(glm::vec3(0.5f, 4.5, -29.5 / 0.1), glm::vec3(1.0, 1.0, 0.1), 90, glm::vec3(0.0, 0.0, 1.0));


	}
}
//
//	//front door, open and closed states
//	if (playerPosition.x > 3 && playerPosition.x < 10) {
//		//open state
//		camera->pushBack(camera->getTop());
//		camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//		camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.1f, 1.2f, 0.1f)));
//		camera->setTop(glm::translate(camera->getTop(), glm::vec3(5.625f, 1.0f, 40.5f)));
//		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//		cube->draw();
//		camera->pop();
//
//	}
//	else {
//		//closed state
//		camera->pushBack(camera->getTop());
//		camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.1f, 1.2f, 0.1f)));
//		camera->setTop(glm::translate(camera->getTop(), glm::vec3(4.625f, 1.0f, -70.0f)));
//		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//		cube->draw();
//		camera->pop();
//
//
//	}
//
//	glBindTexture(GL_TEXTURE_2D, textures[2]); //house wall texture
//	//front of house, left hand side
//	for (int a = 0; a < 2; a++) {
//		for (int b = 0; b < 10; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//			camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 0.125, b*1.2f, -70.0f)));
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//
//			camera->pop();
//		}
//	}
//
//	//draw bed objects
//	{
//		rt3d::setMaterial(mvpShaderProgram, materialDark->getMaterial());
//		glBindTexture(GL_TEXTURE_2D, textures[0]);
//
//		camera->pushBack(camera->getTop());
//		camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f)));
//		camera->setTop(glm::rotate(camera->getTop(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//		camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.00575f, 0.005f, 0.01f)));
//		camera->setTop(glm::translate(camera->getTop(), glm::vec3(5.0f + 0.125, -4000.2f, -70.0f)));
//
//		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//		rt3d::drawMesh(meshObjects[2], mVertCount, GL_TRIANGLES);
//		camera->pop();
//
//		camera->pushBack(camera->getTop());
//		camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f)));
//		camera->setTop(glm::rotate(camera->getTop(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//		camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.00575f, 0.005f, 0.01f)));
//		camera->setTop(glm::translate(camera->getTop(), glm::vec3(-1700.0f + 0.125, -4000.2f, -70.0f)));
//
//		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//		rt3d::drawMesh(meshObjects[2], mVertCount, GL_TRIANGLES);
//		camera->pop();
//
//	}
//
//	//front of house, right hand side
//	glBindTexture(GL_TEXTURE_2D, textures[2]);
//	for (int a = 3; a < 5; a++) {
//		for (int b = 0; b < 10; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//			camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f - 0.275, b*1.2f, -70.0f)));
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//front of house, top side
//	for (int a = 0; a < 5; a++) {
//		for (int b = 10; b < 15; b++) {
//			camera->pushBack(camera->getTop());
//			if (a == 4) {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.125f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 0.875f, b * 1.2f + 0.8125, -70.0f)));
//			}
//			else {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 0.125, b * 1.2f + 0.8125, -70.0f)));
//			}
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//back of house, left hand side
//	for (int a = 0; a < 2; a++) {
//		for (int b = 0; b < 10; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//			camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 0.125, b * 1.2f, -300.0f)));
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//back of house, right hand side
//	for (int a = 3; a < 5; a++) {
//		for (int b = 0; b < 10; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//			camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f - 0.275, b * 1.2f, -300.0f)));
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//back of house, top side
//	for (int a = 0; a < 5; a++) {
//		for (int b = 10; b < 15; b++) {
//			camera->pushBack(camera->getTop());
//			if (a == 4) {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.125f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 0.875f, b * 1.2f + 0.8125, -300.0f)));
//			}
//			else {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 0.125, b * 1.2f + 0.8125, -300.0f)));
//			}
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//left hand side of house
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//			if (b == 15) {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.1f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 2.4f, -10.0f)));
//			}
//			else {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 1.2f, -10.0f)));
//			}
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//left hand side of house (inside)
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//			if (a != 1 && a != 3 && a != 5 && a != 7) {
//				if (b == 15) {
//					camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.1f, 0.1f)));
//					camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 2.4f, 35.0f)));
//				}
//				else {
//					camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//					camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 1.2f, 35.0f)));
//				}
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//				cube->draw();
//				camera->pop();
//			}
//			else {
//				if (b > 9)
//					if (b == 15) {
//						camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.1f, 0.1f)));
//						camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 2.4f, 35.0f)));
//						rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//						cube->draw();
//					}
//					else {
//						camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//						camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 1.2f, 35.0f)));
//						rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//						cube->draw();
//					}
//				camera->pop();
//			}
//		}
//	}
//
//	//right hand side of house (inside)
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//			if (a != 1 && a != 3 && a != 5 && a != 7) {
//				if (b == 15) {
//					camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.1f, 0.1f)));
//					camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 2.4f, 65.0f)));
//				}
//				else {
//					camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//					camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 1.2f, 65.0f)));
//				}
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//				cube->draw();
//				camera->pop();
//			}
//			else {
//				if (b > 9)
//					if (b == 15) {
//						camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.1f, 0.1f)));
//						camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 2.4f, 65.0f)));
//						rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//						cube->draw();
//					}
//					else {
//						camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//						camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 1.2f, 65.0f)));
//						rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//						cube->draw();
//					}
//				camera->pop();
//			}
//		}
//	}
//
//	//right hand side doors
//	glBindTexture(GL_TEXTURE_2D, textures[14]);
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			if (a == 1 || a == 3 || a == 5) {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 1.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(4.2f + 1.875, 1.0f, (a*-25.0f) - 70.0f)));
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			}
//			door->draw();
//			camera->pop();
//		}
//	}
//
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//			if (a == 7) { // removed 7
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 1.2f, 0.1f)));//65
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a*2.0f + 6.575, 1.0f, 65.0f))); //step two change a to z axis, change z axis 
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//
//			}
//			door->draw();
//			camera->pop();
//		}
//	}
//
//	//left hand side doors
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//			if (a == 1 || a == 7) {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 1.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, 1.0f, 35.0f)));
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			}
//			door->draw();
//			camera->pop();
//		}
//	}
//
//	glBindTexture(GL_TEXTURE_2D, textures[2]); // wall texture
//	//right hand side of house
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
//			if (b == 15) {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.1f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 2.4f, 110.0f)));
//			}
//			else {
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 6.575, b * 1.2f, 110.0f)));
//			}
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//inside house, floor
//	glBindTexture(GL_TEXTURE_2D, textures[3]);
//	for (int a = 0; a < 14; a++) {
//		for (int b = 0; b < 4; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f)));
//
//			camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.75f, 1.275f, 0.1f)));
//			if (a == 13) {
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(b * 1.75, (a * -1.225f) - 6.5f, 0.5f)));
//			}
//			else {
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(b * 1.75, (a*-1.275) - 6.5f, 0.5f)));
//			}
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	//inside house, ceiling
//	glBindTexture(GL_TEXTURE_2D, textures[3]); // wall texture
//	for (int a = 0; a < 14; a++) {
//		for (int b = 0; b < 4; b++) {
//			camera->pushBack(camera->getTop());
//			camera->setTop(glm::rotate(camera->getTop(), float(90 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f)));
//
//			camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.675f, 1.275f, 0.1f)));
//			if (a == 13) {
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(b * 1.675 + 0.5, a * -1.225f - 6.5f, -30.5f)));
//			}
//			else {
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(b * 1.675 + 0.5, a * -1.275 - 6.5f, -30.5f)));
//			}
//			rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//			cube->draw();
//			camera->pop();
//		}
//	}
//
//	// left hand interior walls
//	for (int a = 0; a < 2; a++) {
//		for (int b = 0; b < 13; b++) {
//			for (int c = 1; c < 4; c++) {
//				camera->pushBack(camera->getTop());
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.1f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 0.125, b * 1.2f, c * -60.0f - 65.0f)));
//
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//				cube->draw();
//				camera->pop();
//			}
//		}
//	}
//
//	//draw doors
//	glBindTexture(GL_TEXTURE_2D, textures[14]); // door texture
//	for (int a = 0; a < 9; a++) {
//		for (int b = 0; b < 16; b++) {
//			camera->pushBack(camera->getTop());
//			if (a == 1 || a == 3 || a == 5) { // removed 7
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.275f, 1.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(4.2f + 1.875, 1.0f, (a*-25.0f) - 70.0f)));
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//
//			}
//			door->draw();
//			camera->pop();
//		}
//	}
//
//	// right hand interior walls
//	for (int a = 3; a < 5; a++) {
//		for (int b = 0; b < 13; b++) {
//			for (int c = 1; c < 4; c++) {
//				camera->pushBack(camera->getTop());
//				camera->setTop(glm::scale(camera->getTop(), glm::vec3(1.1f, 0.2f, 0.1f)));
//				camera->setTop(glm::translate(camera->getTop(), glm::vec3(a * 2.0f + 1.0, b * 1.2f, c * -60.0f - 65.0f)));
//				rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
//				cube->draw();
//				camera->pop();
//			}
//		}
//	}
//}
void Game::buildColliders() {

	//door collider
	{
		camera->pushBack(camera->getTop());
		door->setTranslate(glm::vec3(30.75f, 0.8f, -5.0f));// z = -x, y doesnt matter
		door->setScale(glm::vec3(1.0, 1.0, 0.3), scaleSetter);
		if (gameWon != true)
		{
			collisionObjs.push_back(door);
		}
		camera->pop();
	}

	//garden wall colliders
	{

		camera->pushBack(camera->getTop());
		wall[10]->setTranslate(glm::vec3(0.0f, 0.8f, -10.0f));
		wall[10]->setScale(glm::vec3(3.5, 1.0, 0.3), scaleSetter);
		collisionObjs.push_back(wall[10]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[11]->setTranslate(glm::vec3(3.5f, 0.8f, 1.5f));
		wall[11]->setScale(glm::vec3(2.0, 1.0, 0.3), scaleSetter);
		collisionObjs.push_back(wall[11]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[12]->setTranslate(glm::vec3(-1.5f, 0.8f, -5.25f));
		wall[12]->setScale(glm::vec3(0.3, 1.0, 7.0), scaleSetter);
		collisionObjs.push_back(wall[12]);
		camera->pop();
	}
	//draw front of house colliders
	{

		//front left
		camera->pushBack(camera->getTop());

		wall[0]->setTranslate(glm::vec3(7.0f, 0.8f, 0.0f));
		wall[0]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);

		//wall[0]->setTranslate(glm::vec3(6.0f, 0.8f, -0.25f));
		//wall[0]->setScale(glm::vec3(0.3, 1.0, 2.5), scaleSetter);

		collisionObjs.push_back(wall[0]);
		camera->pop();


		//front right
		camera->pushBack(camera->getTop());

		wall[1]->setTranslate(glm::vec3(7.0f, 0.8f, -10.0f));
		wall[1]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);

		//wall[1]->setTranslate(glm::vec3(6.0f, 0.8f, -8.5f));
		//wall[1]->setScale(glm::vec3(0.3, 1.0, 1.9), scaleSetter);

		collisionObjs.push_back(wall[1]);
		camera->pop();

	}
	//right hand side wall colliders horizontal
	{

		camera->pushBack(camera->getTop());
		wall[2]->setTranslate(glm::vec3(12.75f, 0.8f, -11.0f));// z = -x, y doesnt matter
		wall[2]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);
		collisionObjs.push_back(wall[2]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[3]->setTranslate(glm::vec3(18.75f, 0.8f, -11.0f));
		wall[3]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);
		collisionObjs.push_back(wall[3]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[4]->setTranslate(glm::vec3(24.75f, 0.8f, -11.0f));
		wall[4]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);
		collisionObjs.push_back(wall[4]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[5]->setTranslate(glm::vec3(30.25f, 0.8f, -11.0f));
		wall[5]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);
		collisionObjs.push_back(wall[5]);
		camera->pop();
	}
	//left hand side wall colliders horizontal
	{
		camera->pushBack(camera->getTop());
		wall[6]->setTranslate(glm::vec3(12.25f, 0.8f, 0.75f));
		wall[6]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);
		collisionObjs.push_back(wall[6]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[7]->setTranslate(glm::vec3(18.5f, 0.8f, 0.25f));
		wall[7]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);
		collisionObjs.push_back(wall[7]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[8]->setTranslate(glm::vec3(24.75f, 0.8f, 0.25f));
		wall[8]->setScale(glm::vec3(0.1, 0.2, 2.5), scaleSetter);
		collisionObjs.push_back(wall[8]);
		camera->pop();

	}
	// left hand side outer wall collider
	{
		rt3d::setMaterial(mvpShaderProgram, materialDark->getMaterial());
		camera->pushBack(camera->getTop());
		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		wall[23]->draw();
		wall[23]->setTranslate(glm::vec3(3.0f, 0.8f, 1.5f));
		wall[23]->setScale(glm::vec3(30.5, 0.2, 0.1), scaleSetter);
		collisionObjs.push_back(wall[23]);
		camera->pop();

	}

	//right hand side outer wall collider
	{
		rt3d::setMaterial(mvpShaderProgram, materialDark->getMaterial());
		camera->pushBack(camera->getTop());
		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		wall[14]->draw();
		wall[14]->setTranslate(glm::vec3(3.0f, 0.8f, -11.5f));
		wall[14]->setScale(glm::vec3(30.5, 1.0, 0.3), scaleSetter);
		collisionObjs.push_back(wall[14]);
		camera->pop();
	}
	//left hand side inner wall colliders
	{
		camera->pushBack(camera->getTop());
		wall[16]->setTranslate(glm::vec3(11.0f, 0.8f, -3.0f));
		wall[16]->setScale(glm::vec3(2.4f, 0.5f, 0.001), scaleSetter);
		collisionObjs.push_back(wall[16]);
		camera->pop();


		camera->pushBack(camera->getTop());
		wall[17]->setTranslate(glm::vec3(18.5f, 0.8f, -2.75f));
		wall[17]->setScale(glm::vec3(1.0f, 1.0f, 0.000175f), scaleSetter);
		collisionObjs.push_back(wall[17]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[18]->setTranslate(glm::vec3(23.5f, 0.8f, -2.75f));
		wall[18]->setScale(glm::vec3(1.3f, 1.0f, 0.000175f), scaleSetter);
		collisionObjs.push_back(wall[18]);
		camera->pop();
	}
	//right hand side inner wall colliders
	{

		camera->pushBack(camera->getTop());
		wall[22]->setTranslate(glm::vec3(9.0f, 0.8f, -7.0f));
		wall[22]->setScale(glm::vec3(0.75, 1.0, 0.000175), scaleSetter);
		collisionObjs.push_back(wall[22]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[21]->setTranslate(glm::vec3(13.5f, 0.8f, -7.0f));
		wall[21]->setScale(glm::vec3(1.3f, 1.0, 0.000175), scaleSetter);
		collisionObjs.push_back(wall[21]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[20]->setTranslate(glm::vec3(18.5f, 0.8f, -7.0f));
		wall[20]->setScale(glm::vec3(1.2, 1.0, 0.000175), scaleSetter);
		collisionObjs.push_back(wall[20]);
		camera->pop();

		camera->pushBack(camera->getTop());
		wall[19]->setTranslate(glm::vec3(28.0f, 0.8f, -7.0f));
		wall[19]->setScale(glm::vec3(-3.3, 1.0, -0.000175), scaleSetter);
		collisionObjs.push_back(wall[19]);
		camera->pop();
	}

}
void Game::render() {

	if (playerPosition.x >= 30.0f) {
		gameExit = true;
		enemyForwardMove = 0.0f;
	}
	if (Score == 3) {
		gameWon = true;
	}
	//set up window for rendering and initialise camera and lighting
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(mvpShaderProgram);
	light = new Light(1);

	rt3d::setLight(mvpShaderProgram, light->getLight());
	rt3d::setMaterial(mvpShaderProgram, material->getMaterial());

	glm::vec4 tmp = camera->getTop()*light->getLightPos();
	rt3d::setLightPos(mvpShaderProgram, glm::value_ptr(tmp));

	camera->pushModelview();

	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 1400.0f / 800.0f, 1.0f, 500.0f);
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "projection", glm::value_ptr(projection));

	glm::mat4 modelview(1.0); // set base position for scene
	camera->pushModelview();
	tmp = camera->getTop()*light->getLightPos();

	float r_new = (90 - r) - 180;
	glm::vec3 vec_offset = { -std::cos(r_new) * (-2.0), 2.0f, std::sin(r_new)* (-2.0) };
	at = { playerPosition.z, playerPosition.y + yTurn, -playerPosition.x };
	eye = at + vec_offset;

	camera->setTop(glm::lookAt(eye, at, up));
	drawSkyBox();

	glDepthMask(GL_TRUE); // make sure depth test is on
	glUseProgram(mvpShaderProgram);

	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 1400.0f / 800.0f, 1.0f, 500.0f);
	rt3d::setUniformMatrix4fv(mvpShaderProgram, "projection", glm::value_ptr(projection));

	rt3d::setLightPos(mvpShaderProgram, glm::value_ptr(tmp));
	rt3d::setMaterial(mvpShaderProgram, materialDark->getMaterial());
	glBindTexture(GL_TEXTURE_2D, textures[12]);
	camera->pop();
	renderMap();

	rt3d::materialStruct tmpMaterial = materialDark->getMaterial();
	rt3d::materialStruct transparentMaterial = materialTransparent->getMaterial();
	// draw the player
	{
		glCullFace(GL_FRONT);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		rt3d::setMaterial(mvpShaderProgram, tmpMaterial);
		player->animate();
		camera->pushBack(camera->getTop());
		camera->setTop(glm::rotate(camera->getTop(), float(90.0f*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
		cube->setTranslate(glm::vec3(1.0f + forwardMove, 0.8f, 5.0f + rightMove));
		camera->setTop(glm::translate(camera->getTop(), glm::vec3(0.0 + forwardMove, 0.0, 0.0 + rightMove)));
		playerPosition = glm::vec3(0.0 + forwardMove, 0.0, 0.0 + rightMove); // set player position for automatic doors
		camera->setTop(glm::rotate(camera->getTop(), float(90.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f)));
		camera->setTop(glm::rotate(camera->getTop(), float(xTurn*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, -1.0f)));
		//camera->setTop(glm::rotate(camera->getTop(), float(yTurn*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
		camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.03, 0.03, 0.03)));
		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		player->draw();

		// set player collider

		rt3d::setMaterial(mvpShaderProgram, transparentMaterial);
		camera->setTop(glm::scale(camera->getTop(), glm::vec3(10.0, 10.0, 30.0)));
		camera->setTop(glm::translate(camera->getTop(), glm::vec3(1.5, 0.0, 0.0))); // set bounding box position
		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		cube->setScale(glm::vec3(1.0f, 1.0f, 1.2f), scaleSetter);
		collisionObjs.push_back(cube);
		camera->pop();
	}
	rt3d::setMaterial(mvpShaderProgram, tmpMaterial); // set back to normal material

	//draw the enemy
	if (Score == 3)
	{

		glBindTexture(GL_TEXTURE_2D, textures[1]);
		enemy->setCurrentAnim(1);
		enemy->animate();
		camera->pushBack(camera->getTop());
		camera->setTop(glm::rotate(camera->getTop(), float(90.0f*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f)));
		camera->setTop(glm::translate(camera->getTop(), glm::vec3(1.0f + enemyForwardMove, 0.8f, 5.0f + enemyRightMove)));
		camera->setTop(glm::rotate(camera->getTop(), float(90.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f)));
		camera->setTop(glm::rotate(camera->getTop(), float(DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, -1.0f)));
		camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.03, 0.03, 0.03)));
		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		enemy->draw();
		camera->pop();

		// enemy collider
		camera->pushBack(camera->getTop());
		rt3d::setUniformMatrix4fv(mvpShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		enemyCollider->setTranslate(glm::vec3(1.5 + enemyForwardMove, 0.5f, -5.0f - enemyRightMove));
		enemyCollider->setScale(glm::vec3(0.5, 1.0, 0.5), scaleSetter);
		collisionObjs.push_back(enemyCollider);
		camera->pop();

		enemyForwardMove += 0.1;


	}
	glDepthMask(GL_TRUE);

	//initialise labels
	{
		stringstream score;       // xTurns the score from an int into a char*
		score << Score;
		string scoreString = score.str();
		char* scoreText = (char*)scoreString.c_str();

		glUseProgram(skyBoxShaderProgram);//Use texture-only shader for text rendering
		glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
		labels[0] = textToTexture("Score: ", labels[0], 0, 0, 0); // set text for HUD
		labels[1] = textToTexture(scoreText, labels[1], 0, 0, 0); // set actual score
		labels[2] = textToTexture("Horror Simulation", labels[2], 0, 0, 0); // title
		labels[3] = textToTexture("You Win", labels[3], 0, 1, 0); // if you win
		labels[4] = textToTexture("You Lose", labels[4], 1, 0, 0); // if you lose
		labels[5] = textToTexture("press ESC to exit", labels[5], 1, 0, 0); // if you win or lose

	}
	//draw labels
	{
		glBindTexture(GL_TEXTURE_2D, labels[0]);
		camera->pushModelview();
		camera->setTop(glm::translate(camera->getTop(), glm::vec3(-0.8f, -0.9f, 0.0f)));
		camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.20f, 0.1f, 0.0f)));
		rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
		rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		rt3d::drawIndexedMesh(meshObjects[0], 12, GL_TRIANGLES);
		camera->pop();

		glBindTexture(GL_TEXTURE_2D, labels[1]);
		camera->pushModelview();
		camera->setTop(glm::translate(camera->getTop(), glm::vec3(-0.55f, -0.91f, 0.0f)));
		camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.05f, 0.09f, 0.0f)));
		rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
		rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		rt3d::drawIndexedMesh(meshObjects[0], 12, GL_TRIANGLES);
		camera->pop();

		glBindTexture(GL_TEXTURE_2D, labels[2]);
		camera->pushModelview();
		camera->setTop(glm::translate(camera->getTop(), glm::vec3(0.05f, 0.91f, 0.0f)));
		camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.5f, 0.09f, 0.0f)));
		rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
		rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "modelview", glm::value_ptr(camera->getTop()));
		rt3d::drawIndexedMesh(meshObjects[0], 12, GL_TRIANGLES);
		camera->pop();

		//if you have won display this message
		if (gameExit == true) {
			glBindTexture(GL_TEXTURE_2D, labels[3]);
			camera->pushModelview();
			camera->setTop(glm::translate(camera->getTop(), glm::vec3(0.05f, 0.5f, 0.0f)));
			camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.5f, 0.09f, 0.0f)));
			rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
			rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "modelview", glm::value_ptr(camera->getTop()));

			rt3d::drawIndexedMesh(meshObjects[0], 12, GL_TRIANGLES);
			camera->pop();

		}
		// if you have lost display this message
		if (gameLost == true) {
			glBindTexture(GL_TEXTURE_2D, labels[4]);
			camera->pushModelview();
			camera->setTop(glm::translate(camera->getTop(), glm::vec3(0.05f, 0.5f, 0.0f)));
			camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.5f, 0.09f, 0.0f)));
			rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
			rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "modelview", glm::value_ptr(camera->getTop()));

			rt3d::drawIndexedMesh(meshObjects[0], 12, GL_TRIANGLES);
			camera->pop();

		}
		//display the escape button label
		if (gameExit == true || gameLost == true)
		{
			glBindTexture(GL_TEXTURE_2D, labels[5]);
			camera->pushModelview();
			camera->setTop(glm::translate(camera->getTop(), glm::vec3(0.05f, 0.3f, 0.0f)));
			camera->setTop(glm::scale(camera->getTop(), glm::vec3(0.25f, 0.045f, 0.0f)));
			rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
			rt3d::setUniformMatrix4fv(skyBoxShaderProgram, "modelview", glm::value_ptr(camera->getTop()));

			rt3d::drawIndexedMesh(meshObjects[0], 12, GL_TRIANGLES);
			camera->pop();
		}
	}
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label
	glDepthMask(GL_TRUE);

	int ob1 = 0, ob2 = 0; // for calculating vector positions

	for (vector<CollisionEntity*>::iterator it_obj1 = collisionObjs.begin(); it_obj1 < collisionObjs.end(); it_obj1++) // cycle through all objects
	{
		ob1++; // iteration counter
		for (vector<CollisionEntity*>::iterator it_obj2 = it_obj1 + 1; it_obj2 < collisionObjs.end(); it_obj2++) // only checking same object collisions once
		{
			ob2++; // iteration counter
			if (checkCollision(*it_obj1, *it_obj2, ob1, ob2) == true) {

				// code for wall collisions
				if (direction == 1)
				{
					eye = moveForward(eye, r, -0.1f), forwardMove -= 0.1; // move in opposite direction
				}
				if (direction == 2)
				{
					eye = moveForward(eye, r, 0.1f), forwardMove += 0.1;
				}
				if (direction == 3)
				{
					eye = moveRight(eye, r, 0.1f), rightMove += 0.1;
				}
				if (direction == 4)
				{
					eye = moveRight(eye, r, -0.1f), rightMove -= 0.1;
				}

			}
		}
	}

	collisionObjs.clear();
	scaleSetter = 1; // render initialisation
	SDL_GL_SwapWindow(hWindow); // swap buffers
}
bool Game::checkCollision(CollisionEntity* cube1, CollisionEntity* cube2, int position1, int position2)

{

	float centreZero = cube1->getCentre(0) - cube2->getCentre(0);
	float centreOne = cube1->getCentre(1) - cube2->getCentre(1);
	float centreTwo = cube1->getCentre(2) + cube2->getCentre(2);


	if (centreZero < 0) centreZero = -centreZero;
	if (centreOne < 0) centreOne = -centreOne;
	if (centreTwo < 0) centreTwo = -centreTwo;

	if (centreTwo >= cube1->getLength() + cube2->getLength() * 2)
	{

		return false;
	}

	if (centreZero >= cube1->getWidth() + cube2->getWidth() * 2)
	{
		return false;
	}

	//there has been a collision

	//check if one is a coin
	if (dynamic_cast<CoinCollider*>(cube1) != nullptr)
	{
		hideCoin(position1);
		return false;

	}

	if (dynamic_cast<CoinCollider*>(cube2) != nullptr)
	{
		hideCoin(position2);
		return false;
	}

	// check if two walls are touching and ignore it
	if (dynamic_cast<WallCollider*>(cube1) != nullptr && dynamic_cast<WallCollider*>(cube2) != nullptr)
	{
		return false;
	}
	//check if enemy and player are touching
	if ((dynamic_cast<EnemyCollider*>(cube1) != nullptr && dynamic_cast<Cube*>(cube2) != nullptr)
		|| ((dynamic_cast<EnemyCollider*>(cube2) != nullptr && dynamic_cast<Cube*>(cube1) != nullptr)))
	{
		gameLost = true;
		return false;
	}
	//check if wall and door are colliding
	if (dynamic_cast<WallCollider*>(cube1) != nullptr && dynamic_cast<DoorCollider*>(cube2) != nullptr)
	{
		return false;
	}


	return true;
}
void Game::hideCoin(int position)
{
	if (coin[position - 1]->getVisible() == true) {
		coin[position - 1]->setVisible(false);
		Score++;
	}
}