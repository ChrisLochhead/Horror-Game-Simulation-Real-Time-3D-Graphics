#pragma once
#include "Cube.h"

class SkyBox : public Cube {

public:
	SkyBox()
	{
		SkyBoxMesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(),
			tex_coords.data(), size, indices.data());
	}


	GLuint getMesh() { return SkyBoxMesh; }

	GLuint getIndexCount() { return IndexCount; }
private:
	GLuint SkyBoxMesh;
	GLuint IndexCount = 6;
};
