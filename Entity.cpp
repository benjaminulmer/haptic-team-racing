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

// Sets the texure for the mesh
void Entity::setTexture(std::string filename) {

	chai3d::cMesh* m = mesh->getMesh(0);

	// Create a colour texture map for this mesh object
	chai3d::cTexture2dPtr texture = chai3d::cTexture2d::create();
	texture->loadFromFile(filename);
	texture->setWrapModeS(GL_REPEAT);
	texture->setWrapModeT(GL_REPEAT);
	texture->setUseMipmaps(true);

	// Assign textures to the mesh
	m->m_texture = texture;
	m->setUseTexture(true);
}

// Returns the view
View Entity::getView() const {
	return view;
}