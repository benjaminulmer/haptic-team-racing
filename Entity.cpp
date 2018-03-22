#include "Entity.h"

using namespace chai3d;

Entity::Entity(std::string filename) {
	mesh = new cMultiMesh();
	mesh->loadFromFile(filename.c_str());
	mesh->setStiffness(2000.0);
	mesh->createAABBCollisionDetector(0.01);
	mesh->m_material->setUseHapticShading(true);
}