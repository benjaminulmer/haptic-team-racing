#include "InputHandler.h"

Program* InputHandler::p;

// Must be called before using InputHandler
void InputHandler::setUp(Program* program) {
	p = program;
}

// Callback for GLFW key presses
void InputHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// Debug camera controls
	if (key == GLFW_KEY_UP) {
		p->moveCamera(-1.0);
	}
	else if (key == GLFW_KEY_DOWN) {
		p->moveCamera(21.0);
	}

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
	else if ((key == GLFW_KEY_ENTER) && (p->getState() == State::LOSE || p->getState() == State::WIN)) {
		p->restartGame();
	}
	else if (key == GLFW_KEY_ENTER) {
		p->exitMenu();
	}
	else if ((key == GLFW_KEY_LEFT) || (key == GLFW_KEY_RIGHT)) {
		p->toggleLevelSelect();
	}
}