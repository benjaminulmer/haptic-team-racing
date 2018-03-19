#include "Program.h"

#include <iostream>

#include "InputHandler.h"

HapticsController* Program::next;

// Default constructor for program
Program::Program() :
	simulationRunning(false),
	simulationFinished(false),
	fullscreen(false),
	mirrored(false)
{
	next = nullptr;
	InputHandler::setUp(this);
	printControls();

	// Initialize GLFW library
	if (!glfwInit()) {
		std::cerr << "failed GLFW initialization" << std::endl;
		system("pause");
		exit(-1);
	}
	glfwSetErrorCallback(errorCallback);

	setUpHapticDevices();

	p1View = new PlayerView(*p1Haptics);
	p2View = new PlayerView(*p2Haptics);

	// Initialize GLEW library
	if (glewInit() != GLEW_OK) {
		std::cout << "failed to initialize GLEW library" << std::endl;
		system("pause");
		glfwTerminate();
		exit(-1);
	}
}

// Pretty prints program controls to standard out
void Program::printControls() {
	std::cout << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	std::cout << "CHAI3D" << std::endl;
	std::cout << "-----------------------------------" << std::endl << std::endl << std::endl;
	std::cout << "Keyboard Options:" << std::endl << std::endl;
	std::cout << "[f] - Enable/Disable full screen mode" << std::endl;
	std::cout << "[q/esc] - Exit application" << std::endl;
	std::cout << std::endl << std::endl;
}

// Initializes haptics device
void Program::setUpHapticDevices() {

	// Get, open, and calibrate device
	chai3d::cGenericHapticDevicePtr device1;
	chai3d::cGenericHapticDevicePtr device2;

	handler.getDevice(device1, 0);
	p1Haptics = new HapticsController(device1);

	handler.getDevice(device2, 1);
	p2Haptics = new HapticsController(device2);
}

// Starts the program
void Program::start() {

	// Start the haptics thread and then enter the graphics loop
	next = p1Haptics;
	hapticsThread1.start(startNextHapticsLoop, chai3d::CTHREAD_PRIORITY_HAPTICS);

	next = p2Haptics;
	hapticsThread2.start(startNextHapticsLoop, chai3d::CTHREAD_PRIORITY_HAPTICS);

	mainLoop();
}

// Main loop for running graphics and other non-haptics work
void Program::mainLoop() {

	while (!p1View->shouldClose() && !p2View->shouldClose()) {

		glfwPollEvents();

		p1View->render();
		p2View->render();
	}

	// Clean up
	closeHaptics();
	delete p1View;
	delete p2View;
	delete p1Haptics;
	delete p2Haptics;
	glfwTerminate();
}

// Updates graphics of the program
void Program::updateGraphics() {


}

// Starts the haptics controller loop of "next"
void Program::startNextHapticsLoop() {
	next->start();
}

// Called to close and clean up program
void Program::closeHaptics() {

	// Wait for haptics loop to terminate
	p1Haptics->stop();
	p2Haptics->stop();
	while (!p1Haptics->isFinished() && !p2Haptics->isFinished()) {
		chai3d::cSleepMs(100);
	}
}

//// Sets window size to new parameters
//void Program::setWindowSize(int width, int height) {
//	this->width = width;
//	this->height = height;
//}

//// Toggles fullscreen mode on and off
//void Program::toggleFullscreen() {
//	
//	fullscreen = !fullscreen;
//
//	// Get info about monitor
//	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//
//	// Set fullscreen or window mode
//	if (fullscreen) {
//		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
//		glfwSwapInterval(1);
//	}
//	else {
//		int w = 0.8 * mode->height;
//		int h = 0.5 * mode->height;
//		int x = 0.5 * (mode->width - w);
//		int y = 0.5 * (mode->height - h);
//		glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
//		glfwSwapInterval(1);
//	}
//}

// Callback to print GLFW errors
void Program::errorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}