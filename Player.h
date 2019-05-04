#pragma once
#include "Camera.h"
#include "CoinCollider.h"

class Player
{
public:
	Player()
	{
		mesh[1] = tmpModel.ReadMD2Model("hayden-tris.MD2");
		md2VertCount = tmpModel.getVertDataCount();

	}
	void setCurrentAnim(int newAnim) { currentAnim = newAnim; }

	void animate()
	{
		tmpModel.Animate(currentAnim, 0.15);
		rt3d::updateMesh(mesh[1], RT3D_VERTEX, tmpModel.getAnimVerts(), tmpModel.getVertDataSize());
	}
	void draw()
	{
		rt3d::drawMesh(mesh[1], md2VertCount, GL_TRIANGLES); // initial matrix
	}
	void setCollider(CoinCollider* collider)
	{
		collider->draw();
	}
	CoinCollider* getCollider() { return collider; }

private:
	int score;
	md2model tmpModel;
	GLuint md2VertCount = 0;
	int currentAnim = 0;
	GLuint mesh[1];
	CoinCollider* collider;

};