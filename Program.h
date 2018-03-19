#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

#include "PlayerView.h"
#include "HapticsController.h"

// Main class for storing and running program
class Program {

public:
	Program();
	void start();

	// These will need to move to PlayerView if we want to keep them
	//void setWindowSize(int width, int height);
	//void toggleFullscreen();

private:
	static HapticsController* volatile next;

	PlayerView* p1View;
	PlayerView* p2View;

	chai3d::cHapticDeviceHandler handler;

	HapticsController* p1Haptics;
	HapticsController* p2Haptics;

	chai3d::cThread hapticsThread1;
	chai3d::cThread hapticsThread2;

	bool fullscreen;

	void printControls();
	void mainLoop();
	void closeHaptics();

	static void startNextHapticsLoop();
	static void errorCallback(int error, const char* description);
};