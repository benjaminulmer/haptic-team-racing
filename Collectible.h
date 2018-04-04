#pragma once

#include "Entity.h"
#include "Signal.h"

class Collectible : public Entity {

public:
	Collectible(std::string filename, View view, chai3d::cTransform transform, double timeBonus);

	virtual chai3d::cVector3d interact(chai3d::cToolCursor* tool);

	Signal<double> pickUpCollectible;

private:
	double timeBonus;
};
