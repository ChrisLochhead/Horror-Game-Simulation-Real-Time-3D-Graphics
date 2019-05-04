#pragma once
#pragma once
#include "rt3d.h"
class Material
{
public:
	Material(int type);
	
	rt3d::materialStruct getMaterial() { return material; }
private:

	rt3d::materialStruct material;
	rt3d::materialStruct type1 = { // FOR DARKNESS
		{ 0.2f, 0.2f, 0.2f, 1.0f }, // ambient
	{ 0.2f, 0.2f, 0.2f, 1.0 }, // diffuse
	{ 0.2f, 0.2f, 0.2f, 1.0 }, // specular
	1.0f  // shininess
	};

	rt3d::materialStruct type2 = {
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // ambient
	{ 0.8f, 0.5f, 0.5f, 1.0f }, // diffuse
	{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
	2.0f  // shininess
	};

	rt3d::materialStruct type3 = {
		{ 0.15f, 0.075f, 0.0f, 1.0f }, // ambient
	{ 0.8f, 0.5f, 0.5f, 1.0f }, // diffuse
	{ 1.0f, 0.8f, 0.8f, 1.0f }, // specular
	20.0f  // shininess
	};

	rt3d::materialStruct type4 = {
		{ 0.15f, 0.075f, 0.0f, 0.3f }, // ambient
	{ 0.8f, 0.5f, 0.5f, 0.3f }, // diffuse
	{ 1.0f, 0.8f, 0.8f, 0.3f }, // specular
	10.0f  // shininess
	};
};