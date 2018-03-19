#pragma once

#include "chai3d.h"

// Class that handles the haptic device of one player
class HapticsController {

public:
	HapticsController(chai3d::cGenericHapticDevicePtr device);
	virtual ~HapticsController();

	void setPartner(const HapticsController* partner);

	void start();
	void stop();
	bool isFinished() const;

	chai3d::cVector3d getPosition() const;
	chai3d::cMatrix3d getRotation() const;
	double getFrequency() const;

private:
	chai3d::cGenericHapticDevicePtr device;
	const HapticsController* partner;

	bool running;
	bool finished;

	mutable chai3d::cFrequencyCounter hapticFreq;

	chai3d::cVector3d curPos;
	chai3d::cMatrix3d curRot;
};

