#pragma once

#include "Entity.h"

// Class for a vicous object the exerts a large damping force
class Viscous : public Entity {

public:
	Viscous(std::string filename, View view, chai3d::cTransform transform, double damping);

	virtual chai3d::cVector3d interact(chai3d::cToolCursor* tool);

private:
	double damping;
};

