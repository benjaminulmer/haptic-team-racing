#pragma once

#include "chai3d.h"

#include <map>
#include <vector>

#include "Entity.h"
#include "Signal.h"
#include "ClosedLoopHaptic.h"

// Class that handles the haptic device of one player
class HapticsController {

public:
	HapticsController(chai3d::cGenericHapticDevicePtr device, const std::vector<Entity*>& entities);
	virtual ~HapticsController();

	void setPartner(HapticsController* partner);

	void start();
	void stop();
	bool isFinished() const;

	chai3d::cVector3d getWorldPosition() const;
	double getFrequency() const;
	chai3d::cToolCursor* getCursor();
	chai3d::cShapeSphere* getCursorCopy();

	void addClosedLoopForce(ClosedLoopHaptic* force);
	void setupTool(chai3d::cWorld* w);

	void setPosiiton(chai3d::cVector3d pos);
	void reset();

	Signal<Entity*> destroyEntity;
	Signal<> springBroken;

private:
	chai3d::cGenericHapticDevicePtr device;
	chai3d::cWorld* world;
	HapticsController* partner;
	chai3d::cToolCursor* tool;

	const std::vector<Entity*>& entities;
	std::map<const Entity*, bool> insideEntity;

	std::vector<ClosedLoopHaptic*> closedLoopForces;

	bool springIntact;

	bool running;
	bool finished;

	mutable chai3d::cFrequencyCounter hapticFreq;

	chai3d::cVector3d devicePos;
	chai3d::cVector3d prevWorldPos;

	// Allows other player to see avatar
	chai3d::cShapeSphere* avatarCopy;

	void performEntityInteraction();
	void applySpringForce();
	void performRateControl();
};