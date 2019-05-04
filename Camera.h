#pragma once
#include "glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>

#define DEG_TO_RADIAN 0.017453293

using namespace std;

class Camera
{
public:
	Camera() {

		glm::mat4 view(1.0);
		modelview = view;
		mvStack.push(modelview);

	}
	glm::mat4 getTop() { return mvStack.top(); }
	void setTop(glm::mat4 newValue) { mvStack.top() = newValue; }

	void pushModelview();
	void pushBack(glm::mat4 newTop);
	void pop() { mvStack.pop(); }


	glm::vec3 moveForward(glm::vec3 cam, GLfloat angle, GLfloat d);

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);

private:
	glm::mat4 modelview;
	std::stack<glm::mat4> mvStack;

};