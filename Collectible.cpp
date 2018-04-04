#include "Collectible.h"

Collectible::Collectible(std::string filename, View view, chai3d::cTransform transform, double timeBonus) : Entity(filename, view, transform), timeBonus(timeBonus) {
	type = Type::COLLECTIBLE;
	mesh->getMesh(0)->setHapticEnabled(false);
}

// Emits a signal that a collectible has been picked up (and will add time to the game)
chai3d::cVector3d Collectible::interact(chai3d::cToolCursor* tool) {
	pickUpCollectible.emit(timeBonus);
	return chai3d::cVector3d(0.0, 0.0, 0.0);
}
