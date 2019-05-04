#pragma once
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glew.h"
#include "rt3d.h"
#include "CollisionEntity.h"

#define DEG_TO_RADIAN 0.017453293

class Coin
{
public:
	Coin() {

		rt3d::loadObj("Res/Objects/dogecoin.obj", verts, norms, tex_coords, indices);

		size = indices.size();
		meshIndexCount = indices.size();
		cubeMesh[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());

	}

	void draw() {
		if (visible == true) { rt3d::drawIndexedMesh(cubeMesh[0], meshIndexCount, GL_TRIANGLES); }
	}

	void setVisible(bool newVisible) { visible = newVisible; }
	bool getVisible() { return visible; }
	void rotate(glm::mat4 stackTop) { glm::rotate(stackTop, float(90 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f)); }
	GLuint getMesh() { return cubeMesh[0]; }

protected:

	bool visible = true;
	GLuint meshIndexCount = 0;
	GLuint cubeMesh[1];
	GLuint cubeVertCount = 8;

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;

	GLuint size;
};