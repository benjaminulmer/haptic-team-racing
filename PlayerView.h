#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

#include "HapticsController.h"

// Class that handles the view (window) of one player
class PlayerView {

public:
	PlayerView(const HapticsController& controller);
	virtual ~PlayerView();

	void render();
	bool shouldClose() const;
	const GLFWwindow* getWindow() const;

	void addChild(chai3d::cMultiMesh* mesh);
	chai3d::cWorld* getWorld() { return world; };

private:
	GLFWwindow* window;
	int width;
	int height;

	// Graphics world and objects
	chai3d::cWorld* world;
	chai3d::cCamera* camera;
	chai3d::cSpotLight* light;
	chai3d::cLabel* labelRates;

	const HapticsController& controller;

	chai3d::cFrequencyCounter graphicsFreq;

	void setUpWorld();
};

