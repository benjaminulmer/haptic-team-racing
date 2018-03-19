#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

#include "PlayerView.h"

// Main class for storing and running program (a bit of a god class)
class Program {

public:
	Program();
	void start();

	//void setWindowSize(int width, int height);
	//void toggleFullscreen();

private:
	// Static pointer so haptics thread can access the program object
	// Probably not the best way to do this but whatever
	static Program* p;

	PlayerView* p1View;
	PlayerView* p2View;

	// Graphics world and objects
	chai3d::cWorld* world;
	chai3d::cCamera* camera;
	chai3d::cDirectionalLight* light;
	chai3d::cShapeSphere* cursor;
	chai3d::cLabel* labelRates;

	chai3d::cFrequencyCounter freqCounterGraphics;
	chai3d::cFrequencyCounter freqCounterHaptics;

	chai3d::cHapticDeviceHandler handler;
	chai3d::cGenericHapticDevicePtr hapticDevice;

	// Simulation status and handle
	bool simulationRunning;
	bool simulationFinished;

	chai3d::cThread hapticsThread;

	bool fullscreen;
	bool mirrored;

	void printControls();
	void setUpWorld();
	void setUpHapticDevice();

	void mainLoop();
	void updateGraphics();

	static void hapticsLoop();
	static void close();
	static void errorCallback(int error, const char* description);
};