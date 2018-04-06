#pragma once

#include "ClosedLoopHaptic.h"

// Class for the force applied when a collectible is picked up
class PickupForce : public ClosedLoopHaptic {

public:
	PickupForce();

	virtual chai3d::cVector3d getForce(chai3d::cToolCursor* tool);
};

