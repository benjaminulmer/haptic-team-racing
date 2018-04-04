#include "Hazard.h"

Hazard::Hazard(std::string filename, View view, chai3d::cTransform transform) : Entity(filename, view, transform) {
	type = Type::HAZARD;
	mesh->getMesh(0)->setHapticEnabled(false);
}

// Emits a signal that a hazard has been hit (and will end the game)
chai3d::cVector3d Hazard::interact(chai3d::cToolCursor* tool) {
	hitHazard.emit();
	return chai3d::cVector3d(0.0, 0.0, 0.0);
}
