#include "Viscous.h"

Viscous::Viscous(std::string filename, View view, chai3d::cTransform transform, double damping) : Entity(filename, view, transform), damping(damping) {
	type = Type::VISCOUS;
	mesh->getMesh(0)->setHapticEnabled(false);
}

// Returns a damping force simulating a viscous material like molasses
chai3d::cVector3d Viscous::interact(chai3d::cToolCursor* tool) {
	return tool->getDeviceLocalLinVel() * -damping;
}
