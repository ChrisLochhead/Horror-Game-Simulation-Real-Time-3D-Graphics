#include "Camera.h"

void Camera::pushModelview()
{
	glm::mat4 ModelView(1.0);
	mvStack.push(ModelView);
}
void Camera::pushBack(glm::mat4 newTop)
{
	mvStack.push(newTop);
}

glm::vec3 Camera::moveForward(glm::vec3 cam, GLfloat angle, GLfloat d) {
	return glm::vec3(cam.x + d * std::sin(angle*DEG_TO_RADIAN), cam.y, cam.z - d * std::cos(angle*DEG_TO_RADIAN));
}

glm::vec3 Camera::moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d * std::cos(angle*DEG_TO_RADIAN),
		pos.y, pos.z + d * std::sin(angle*DEG_TO_RADIAN));
}