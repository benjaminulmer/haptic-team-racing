#pragma once

#include "Entity.h"

class Viscous : public Entity {

public:
	Viscous(std::string filename, View view, chai3d::cTransform transform = chai3d::cTransform());

	virtual chai3d::cVector3d interact(chai3d::cToolCursor* tool) const;

private:
	double damping;
};

