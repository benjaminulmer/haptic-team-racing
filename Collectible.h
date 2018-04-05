#pragma once

#include "Entity.h"
#include "Signal.h"

// Class for a collectable object that adds time when picked up
class Collectible : public Entity {

public:
	Collectible(std::string filename, View view, chai3d::cTransform transform, double timeBonus);

	virtual chai3d::cVector3d interact(chai3d::cToolCursor* tool);
	virtual bool destoryOnInteract() { return true; }

	Signal<double> pickUpCollectible;

private:
	double timeBonus;
};
