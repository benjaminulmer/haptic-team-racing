#include "Entity.h"

#include "Constants.h"

// Creates an entity from a file name
Entity::Entity(std::string filename, View view, chai3d::cTransform transform) : view(view) {

	type = Type::ENTITY;

	mesh = new chai3d::cMultiMesh();
	mesh->loadFromFile(filename);
	mesh->setLocalTransform(transform);
	mesh->setStiffness(3000.0);
	mesh->createAABBCollisionDetector(Constants::cursorRadius);
	mesh->m_material->setUseHapticShading(true);

	mesh->createEffectMagnetic();
}

// Deletes entity and its mesh
Entity::~Entity() {
	delete mesh;
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

// Returns the type of the entity
Type Entity::getType() const {
	return type;
}
