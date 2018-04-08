#include "PickupForce.h"

PickupForce::PickupForce() {
	durationS = 0.25;
	clock.start();
}

// Returns high frequncy vibration force
chai3d::cVector3d PickupForce::getForce(chai3d::cToolCursor* tool) {
	
	double timeS = clock.getCurrentTimeSeconds();
	double s = 5.0 * sin(2.0 * M_PI * 120.0 * timeS);

	return chai3d::cVector3d(s, 0.0, 0.0);
}
