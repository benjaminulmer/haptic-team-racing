#pragma once

#include "chai3d.h"

#include <vector>

#include "Entity.h"

// Class that handles the haptic device of one player
class HapticsController {

public:
	HapticsController(chai3d::cGenericHapticDevicePtr device, const std::vector<Entity>& entities);
	virtual ~HapticsController();

	void setPartner(const HapticsController* partner);

	void start();
	void stop();
	bool isFinished() const;

	chai3d::cVector3d getPosition() const;
	chai3d::cVector3d getWorldPosition() const;
	chai3d::cMatrix3d getRotation() const;
	double getFrequency() const;
	chai3d::cToolCursor* getCursor();
	chai3d::cShapeSphere* getCursorCopy();

	void setupTool(chai3d::cWorld* w, chai3d::cCamera* c);

private:
	chai3d::cGenericHapticDevicePtr device;
	chai3d::cWorld* world;
	const HapticsController* partner;
	chai3d::cToolCursor* tool;
	chai3d::cCamera* camera; // Store reference to camera for rate control

	const std::vector<Entity>& entities;

	bool running;
	bool finished;

	mutable chai3d::cFrequencyCounter hapticFreq;

	double k;
	double rest;

	chai3d::cVector3d devicePos;
	chai3d::cVector3d prevWorldPos;

	// Allows other player to see avatar
	chai3d::cShapeSphere* avatarCopy;

	void checkRateControl();
};