#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

#include "PlayerView.h"
#include "HapticsController.h"

// Main class for storing and running program (a bit of a god class)
class Program {

public:
	Program();
	void start();

	//void setWindowSize(int width, int height);
	//void toggleFullscreen();

private:
	static HapticsController* volatile next;

	PlayerView* p1View;
	PlayerView* p2View;

	HapticsController* p1Haptics;
	HapticsController* p2Haptics;

	chai3d::cHapticDeviceHandler handler;

	// Simulation status and handle
	bool simulationRunning;
	bool simulationFinished;

	chai3d::cThread hapticsThread1;
	chai3d::cThread hapticsThread2;

	bool fullscreen;
	bool mirrored;

	void printControls();
	void setUpHapticDevices();

	void mainLoop();

	void closeHaptics();

	static void startNextHapticsLoop();
	static void errorCallback(int error, const char* description);
};