#pragma once

#include "Program.h"

class InputHandler {

public:
	static void setUp(Program* program);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	static Program* p;
};

