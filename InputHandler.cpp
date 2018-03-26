#include "InputHandler.h"

Program* InputHandler::p;

// Must be called before using InputHandler
void InputHandler::setUp(Program* program) {
	p = program;
}

// Callback for GLFW key presses
void InputHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	if (action != GLFW_PRESS) {
		return;
	}
	else if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_F) {
		p->toggleFullscreen();
	}
	else if (key == GLFW_KEY_S) {
		p->swapDevices();
	}
}