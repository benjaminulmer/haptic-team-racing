#include "PlayerView.h"

#include <iostream>

#include "InputHandler.h"

// Creates a GLFW window for the player view
PlayerView::PlayerView() {

	// Get window width and height
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int w = 0.8 * mode->height;
	int h = 0.5 * mode->height;
	int x = 0.5 * (mode->width - w);
	int y = 0.5 * (mode->height - h);

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Create window
	window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
	if (!window) {
		std::cerr << "failed to create GLFW window" << std::endl;
		system("pause");
		glfwTerminate();
		exit(-1);
	}

	// Set window properties
	glfwGetWindowSize(window, &width, &height);
	glfwSetWindowPos(window, x, y);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Set callback functions
	glfwSetKeyCallback(window, InputHandler::keyCallback);
	glfwSetWindowSizeCallback(window, InputHandler::windowSizeCallback);
}

// Closes the GLFW window
PlayerView::~PlayerView() {
	glfwDestroyWindow(window);
}

// Render the current view
void PlayerView::render() {
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

// Returns if the GLFW window should close
bool PlayerView::shouldClose() {
	return glfwWindowShouldClose(window);
}

// Returns constant pointer to the GLFW window of the view
const GLFWwindow * PlayerView::getWindow() {
	return window;;
}