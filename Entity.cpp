#include "Entity.h"

// Creates an entity from a file name
Entity::Entity(std::string filename, View view, chai3d::cTransform transform) : view(view) {

	mesh = new chai3d::cMultiMesh();
	mesh->loadFromFile(filename);
	mesh->setLocalTransform(transform);
	mesh->setStiffness(2000.0);
	mesh->createAABBCollisionDetector(0.01);
	mesh->m_material->setUseHapticShading(true);
}

// Returns the view
View Entity::getView() {
	return view;
}