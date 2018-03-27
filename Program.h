#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

#include "Entity.h"
#include "HapticsController.h"
#include "PlayerView.h"

// Main class for storing and running program
class Program {

public:
	Program();
	void start();

	void toggleFullscreen();
	void swapDevices();

private:
	std::vector<Entity> entities;
	chai3d::cWorld* world;

	PlayerView* p1View;
	PlayerView* p2View;

	chai3d::cHapticDeviceHandler handler;

	HapticsController* p1Haptics;
	HapticsController* p2Haptics;

	chai3d::cThread hapticsThread1;
	chai3d::cThread hapticsThread2;

	int numMonitors;
	bool fullscreen;

	void printControls();
	void setUpHapticDevices();
	void setUpViews();

	void mainLoop();
	void closeHaptics();

	// Static members
	static HapticsController* volatile next;

	static void startNextHapticsLoop();
	static void errorCallback(int error, const char* description);
};