#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

// Class that handles the view (window) of one player
class PlayerView {

public:
	PlayerView();
	virtual ~PlayerView();

	void render();

	bool shouldClose();
	const GLFWwindow* getWindow();

private:
	GLFWwindow* window;
	int width;
	int height;

};

