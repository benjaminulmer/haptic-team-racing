#include "Hazard.h"

Hazard::Hazard(std::string filename, View view, chai3d::cTransform transform) : Entity(filename, view, transform) {
	type = Type::HAZARD;
	mesh->getMesh(0)->setHapticEnabled(false);
}

// Emits a signal that a hazard has been hit (and will end the game)
chai3d::cVector3d Hazard::interact(chai3d::cToolCursor* tool) {
	
	hitHazard.emit();

	chai3d::cVector3d hazPos = mesh->getGlobalPos();
	chai3d::cVector3d toolPos = tool->getLocalTransform() * tool->m_hapticPoint->m_sphereProxy->getLocalPos();

	chai3d::cVector3d dir = chai3d::cNormalize(toolPos - hazPos);

	//return dir * 10.0;
	return chai3d::cVector3d(0.0, 0.0, 0.0);
}
