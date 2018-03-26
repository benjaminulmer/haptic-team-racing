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

// Copy constructor
Entity::Entity(const Entity & other) {

	isRenderable = other.isRenderable;
	pos = other.pos;
	mesh = other.mesh->copy(true, true, true, true);
	view = other.view;
}

// Copy assignment operator
Entity& Entity::operator=(const Entity& other) {
	
	if (&other == this) {
		return *this;
	}
	isRenderable = other.isRenderable;
	pos = other.pos;
	mesh = other.mesh->copy(true, true, true, true);
	view = other.view;

	return *this;
}

// Returns the view
View Entity::getView() {
	return view;
}