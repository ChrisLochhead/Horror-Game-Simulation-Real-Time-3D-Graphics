#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glew.h"
#include "rt3d.h"

class Light {

public:

	Light(int type)
	{
		if (type == 1)
		{
			light = type1;
		}
	}
	rt3d::lightStruct getLight() { return light; }
	glm::vec4 getLightPos() { return lightPos; }
private:

	rt3d::lightStruct light;

	rt3d::lightStruct type1 = { // dark and gloomy
		{ 0.15f, 0.15f, 0.15f, 1.0f }, // ambient colour of light
	{ 0.6f, 0.6f, 0.6f, 1.0f }, // diffuse wideness of spot
	{ 0.5f, 0.5f, 0.5f, 1.0f }, // specular intesity of spot
	{ lightx, lighty, lightz, lighta }  // position
	};

	rt3d::lightStruct type2 = { // Test light
		{ 0.6f, 0.6f, 0.6f, 1.0f }, // ambient
	{ 0.0f, 0.0f, 0.0f, 0.0f }, // diffuse
	{ 10.5f, 10.5f, 10.5f, 1.0f }, // specular
	{ lightx, lighty, lightz, lighta }  // position
	};


	GLfloat lightx = 0.0f;
	GLfloat lighty = 0.0f;
	GLfloat lightz = -2.0f;
	GLfloat lighta = 1.0f;

	glm::vec4 lightPos = { lightx, lighty, lightz, lighta };

};