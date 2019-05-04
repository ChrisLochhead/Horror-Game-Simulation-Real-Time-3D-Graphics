#pragma once
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glew.h"
#include "rt3d.h"


#define DEG_TO_RADIAN 0.017453293

class Bed {
public:
	Bed()
	{
		rt3d::loadObj("Res/Objects/bed.obj", verts, norms, tex_coords, indices);
		size = indices.size();
		meshIndexCount = indices.size();
		cubeMesh[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());
	}
private:
	GLuint meshIndexCount = 0;
	GLuint cubeMesh[1];
	GLuint cubeVertCount = 8;

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;

	GLuint size;
};