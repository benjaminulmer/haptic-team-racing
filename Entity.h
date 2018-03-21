#pragma once

#include "chai3d.h"

class Entity {
public:
	Entity(std::string filename);

	chai3d::cVector3d getForce();

	chai3d::cMultiMesh* mesh;

private:
	std::string filename;

	bool isRenderable;
	chai3d::cVector3d pos;
};