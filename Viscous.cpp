#include "Viscous.h"

Viscous::Viscous(std::string filename, View view, chai3d::cTransform transform) : Entity(filename, view, transform) {
	damping = 50.0;
}

// Returns a damping force simulating a viscous material like molasses
chai3d::cVector3d Viscous::interact(chai3d::cToolCursor* tool) const {
	return tool->getDeviceLocalLinVel() * -damping;
}
