#pragma once

#include "chai3d.h"

enum class View {
	P1 = 1,
	P2 = 2,
	BOTH = 3
};

enum class Type {
	ENTITY,
	VISCOUS,
	HAZARD,
	COLLECTIBLE,
	MAGNET
};

// Base class for an entity. Represents a solid object interacted with via the God-Object algorithm
class Entity {

public:
	Entity(std::string filename, View view, chai3d::cTransform transform);
	virtual ~Entity();

	chai3d::cMultiMesh* mesh;

	void setTexture(std::string filename);
	View getView() const;
	Type getType() const;
	virtual chai3d::cVector3d interact(chai3d::cToolCursor* tool) { return chai3d::cVector3d(0.0, 0.0, 0.0); }
	virtual bool insideForInteraction() { return true; }
	virtual bool destoryOnInteract() { return false; }

protected:
	View view;
	Type type;
};