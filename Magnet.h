#pragma once

#include "Entity.h"

// Class for a magnetic object that attracts the cursor
class Magnet : public Entity {

public:
	Magnet(std::string filename, View view, chai3d::cTransform transform, double strength);

	virtual chai3d::cVector3d interact(chai3d::cToolCursor* tool);
	virtual bool insideForInteraction() { return false; }

private:
	double strength;
};

