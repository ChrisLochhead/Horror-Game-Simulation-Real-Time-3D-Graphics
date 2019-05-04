#pragma once
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glew.h"
#include "rt3d.h"
#include "CollisionEntity.h"


#define DEG_TO_RADIAN 0.017453293

class Cube : public CollisionEntity
{
public:
	Cube() {

		rt3d::loadObj("Res/Objects/cube.obj", verts, norms, tex_coords, indices);
	
		size = indices.size();
		meshIndexCount = indices.size();
		cubeMesh[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());
		centre = { 0.0f, 0.0f, 0.0f };
		halfLength = 0.5f;
	}

	void update()
	{

	}
	void draw() {
		rt3d::drawIndexedMesh(cubeMesh[0], meshIndexCount, GL_TRIANGLES);
	}
	
	void rotate(glm::mat4 stackTop)
	{
		glm::rotate(stackTop, float(90 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 moveForward(glm::vec3 cam, GLfloat angle, GLfloat d) {
		return glm::vec3(cam.x + d * std::sin(angle*DEG_TO_RADIAN),
			cam.y, cam.z - d * std::cos(angle*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d * std::cos(angle*DEG_TO_RADIAN),
			pos.y, pos.z + d * std::sin(angle*DEG_TO_RADIAN));
	}
	void setScale(glm::vec3 newScale, int ScaleSetter)
	{
		if (ScaleSetter == 0) {
			halfWidth *= newScale.x;
			halfHeight *= newScale.y;
			halfLength *= newScale.z;
		}

	}
	void setTranslate(glm::vec3 newPosition)
	{
		centre.x = newPosition.x;
		centre.y = newPosition.y;
		centre.z = newPosition.z;
	}
    float getWidth() { return halfWidth; }
	float getHeight() { return halfHeight; }
	float getLength() { return halfLength; }
	float getCentre(int type)
	{
		if (type == 0) {

			return centre.x; 
		}
		if (type == 1)
		{
			return centre.y;
		}
		if (type == 2)
		{
			return centre.z;
		}
 }
	GLuint getMesh() { return cubeMesh[0]; }

protected:

	GLuint meshIndexCount = 0;
	GLuint cubeMesh[1];
	GLuint cubeVertCount = 8;

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;


	glm::vec3 centre; 


	GLuint size;
private:
	float halfLength;// = 0.5f;
	float halfHeight = 0.5f;
	float halfWidth = 0.5f;
};