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

using namespace std;

bool handleSDLEvent(SDL_Event const &sdlEvent)
{

	if (sdlEvent.type == SDL_QUIT)
		return false;
	if (sdlEvent.type == SDL_KEYDOWN)
	{
		// Can extend this to handle a wider range of keys
		switch (sdlEvent.key.keysym.sym)
		{
		case SDLK_ESCAPE:

			return false;
		default:
			break;
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
	Game* game = new Game();
	SDL_GLContext glContext; // OpenGL context handle
	game->Window(glContext); // Create window and render context 
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << endl;
		exit(1);
	}
	cout << glGetString(GL_VERSION) << endl;

	game->init();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop

		running = handleSDLEvent(sdlEvent);
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;
		}
		game->update();
		game->render(); // call the draw function
	}
	BASS_Free();
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(game->getWindow());
	SDL_Quit();
	return 0;
}
