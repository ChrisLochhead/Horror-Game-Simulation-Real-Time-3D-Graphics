#pragma once
#include "rt3d.h"
#include "Camera.h"

class Enemy
{
public:
	Enemy()
	{
		mesh[1] = enemyModel.ReadMD2Model("hueteotl.MD2");
		md2VertCount = enemyModel.getVertDataCount();

	}
	void setCurrentAnim(int newAnim) { currentAnim = newAnim; }

	void animate()
	{
		enemyModel.Animate(currentAnim, 0.15);
		rt3d::updateMesh(mesh[1], RT3D_VERTEX, enemyModel.getAnimVerts(), enemyModel.getVertDataSize());
	}
	void draw() { rt3d::drawMesh(mesh[1], md2VertCount, GL_TRIANGLES); }


private:
	int score;
	md2model enemyModel;
	GLuint md2VertCount = 0;
	int currentAnim = 0;
	GLuint mesh[1];

};