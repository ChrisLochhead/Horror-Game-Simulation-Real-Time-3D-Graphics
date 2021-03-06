#pragma once
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glew.h"
#include "rt3d.h"
#include "CollisionEntity.h"
#include <vector>

#define DEG_TO_RADIAN 0.017453293

class WallCollider: public CollisionEntity
{
public:
	WallCollider() {

		rt3d::loadObj("Res/Objects/cube.obj", verts, norms, tex_coords, indices);

		size = indices.size();
		meshIndexCount = indices.size();
		cubeMesh[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());
		centre = { 0.0f, 0.0f, 0.0f };
		halfLength = 0.5f;

	}

	void draw() {
		rt3d::drawIndexedMesh(cubeMesh[0], meshIndexCount, GL_TRIANGLES);
	}

	void setScale(glm::vec3 newScale);

	void setTranslate(glm::vec3 newPosition);

	float getWidth() { return halfWidth; }
	float getHeight() { return halfHeight; }
	float getLength() { return halfLength; }


	float getCentre(int type);
	
	GLuint getMesh() { return cubeMesh[0]; }

protected:

	GLuint meshIndexCount = 0;
	GLuint cubeMesh[1];
	GLuint cubeVertCount = 8;

	std::vector<GLfloat> verts;
	std::vector<GLfloat> norms;
	std::vector<GLfloat> tex_coords;
	std::vector<GLuint> indices;


	glm::vec3 centre;


	GLuint size;
private:
	float halfLength = 0.5f;
	float halfHeight = 0.5f;
	float halfWidth = 0.5f;
};