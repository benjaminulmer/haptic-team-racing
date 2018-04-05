#pragma once

#include "Entity.h"
#include "Signal.h"

// Class for a hazrd the ends the game when interacted with
class Hazard : public Entity {

public:
	Hazard(std::string filename, View view, chai3d::cTransform transform);

	virtual chai3d::cVector3d interact(chai3d::cToolCursor* tool);
	virtual bool destoryOnInteract() { return true; }

	Signal<> hitHazard;
};

