#include "BombForce.h"

// Contructs a bomb force centred at the provided position
BombForce::BombForce(chai3d::cVector3d pos) : pos(pos) {
	durationS = 0.15;
	clock.start();
}

// Returns a inverse square repulsion force from the detonation location plus a mid frequency strong vibration
chai3d::cVector3d BombForce::getForce(chai3d::cToolCursor* tool) {

	chai3d::cVector3d toolPos = tool->getLocalTransform() * tool->m_hapticPoint->m_sphereProxy->getLocalPos();
	chai3d::cVector3d dir = toolPos - pos;
	double dist = dir.length();
	dir.normalize();

	double timeS = clock.getCurrentTimeSeconds();

	// Linear ramp up and down on force
	double strength = 0.01;
	if (timeS < durationS * 0.1) {
		strength *= timeS / (durationS * 0.1);
	}
	else if (timeS > durationS * 0.9) {
		strength *= (durationS - timeS) / (durationS - durationS * 0.9);
	}

	// Calculate force
	chai3d::cVector3d force;
	force = dir * strength / (dist * dist);

	if (force.length() > 15.0) {
		force.normalize();
		force *= 15.0;
	}

	double s = strength * 1000.0 * sin(2.0 * M_PI * 60.0 * timeS);
	return force + chai3d::cVector3d(s, 0.0, 0.0);
}
