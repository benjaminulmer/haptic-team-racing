#pragma once

#include "chai3d.h"

// Abstract base class for a closed haptic force
class ClosedLoopHaptic {

public:
	virtual chai3d::cVector3d getForce(chai3d::cToolCursor* tool) = 0;
	virtual bool done() { return clock.getCurrentTimeSeconds() >= durationS; }

protected:
	chai3d::cPrecisionClock clock;
	double durationS;
};

