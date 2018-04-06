#pragma once

#include "ClosedLoopHaptic.h"

// Class for the force applied when a bomb explodes
class PickupForce : public ClosedLoopHaptic {

public:
	PickupForce();

	virtual chai3d::cVector3d getForce(chai3d::cToolCursor* tool);
};

