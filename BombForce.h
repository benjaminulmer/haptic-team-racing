#pragma once

#include "ClosedLoopHaptic.h"

// Class for the force applied when a bomb explodes
class BombForce : public ClosedLoopHaptic {

public:
	BombForce(chai3d::cVector3d pos);

	virtual chai3d::cVector3d getForce(chai3d::cToolCursor* tool);

private:
	chai3d::cVector3d pos;
};

