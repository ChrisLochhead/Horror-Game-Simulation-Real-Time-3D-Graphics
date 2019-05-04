#pragma once
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glew.h"
#include "rt3d.h"

class CollisionEntity
{

public:

	virtual GLfloat getWidth() = 0;
	virtual GLfloat getHeight() = 0;
	virtual GLfloat getLength() = 0;
	virtual float getCentre(int type) = 0;
	void setVisible(bool newVisible);
};