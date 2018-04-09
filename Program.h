#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

#include "Entity.h"
#include "HapticsController.h"
#include "PlayerView.h"
#include "Signal.h"

enum class State {
	RUNNING,
	WIN,
	LOSE,
	END,
	DEFAULT
};

// Main class for storing and running program
class Program {

public:
	Program();
	void start();

	void toggleFullscreen();
	void swapDevices();

	// Debug camera controls
	void moveCamera(double direction);

	void exitMenu() { inMenu = false; };
	void toggleLevelSelect();
	void restartGame();
	State getState() { return state; };

private:
	std::vector<Entity*> entities;
	chai3d::cWorld* world;

	PlayerView* p1View;
	PlayerView* p2View;
	PlayerView* menuView;

	GLFWmonitor** monitors;

	chai3d::cHapticDeviceHandler handler;

	HapticsController* p1Haptics;
	HapticsController* p2Haptics;

	chai3d::cThread hapticsThread1;
	chai3d::cThread hapticsThread2;

	int numMonitors;
	bool fullscreen;

	bool inMenu;
	int levelSelect;

	std::string selectedLevel;
	chai3d::cVector3d startPos;
	chai3d::cPrecisionClock clock;

	// Game state variables
	State state;
	double maxTime;

	void printControls();
	void setUpHapticDevices();
	void setUpViews();
	void setUpMenu();

	void mainLoop();
	void menuLoop();
	void loseGame();
	void winGame();
	void addTime(double amount);
	void destroyEntity(Entity* entity);

	void closeHaptics();

	// Static members
	static HapticsController* volatile next;

	static void startNextHapticsLoop();
	static void errorCallback(int error, const char* description);
};