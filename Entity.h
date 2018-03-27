#pragma once

#include "chai3d.h"

enum class View {
	P1 = 1,
	P2 = 2
};

class Entity {

public:
	Entity(std::string filename, View view, chai3d::cTransform transform = chai3d::cTransform());

	chai3d::cMultiMesh* mesh;

	void setTexture(std::string filename);
	View getView() const;
	chai3d::cVector3d getForce();

private:

	bool isRenderable;
	chai3d::cVector3d pos;

	View view;
};