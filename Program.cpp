#include "Program.h"

#include <iostream>
#include <string>

#include "InputHandler.h"
#include "ContentReadWrite.h"
#include "WorldLoader.h"

HapticsController* volatile Program::next;

// Default constructor for program
Program::Program() {

	fullscreen = true;
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

	// Set up haptic devices
	setUpHapticDevices();
	setUpViews();

	p1Haptics->setupTool(p1View->getWorld(), p1View->getCamera());
	p2Haptics->setupTool(p2View->getWorld(), p2View->getCamera());

	// Temporarily load level here
	WorldLoader::loadWorld(ContentReadWrite::readJSON("worlds/sampleWorld.json"), entities);

	for (Entity e : entities) {
		if (e.getView() == View::P1) {
			p1View->addChild(e.mesh);
		}
		else {
			p2View->addChild(e.mesh);
		}
	}

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
	std::cout << "[f] - Enable/Disable full screen mode - not working" << std::endl;
	std::cout << "[q/esc] - Exit application" << std::endl;
	std::cout << std::endl << std::endl;
}

// Prompts for two haptic devices to be connected then sets up the devices
void Program::setUpHapticDevices() {

	while (handler.getNumDevices() < 2) {

		std::cout << "Game requires two haptic devices to play" << std::endl;
		std::cout << "Enter \"H\" to continue without the devices, or anything else to try again" << std::endl;
		std::string in;
		std::cin >> in;

		if (in == "h" || in == "H") {
			break;
		}

		// Recreate handler to look for more devices
		handler = chai3d::cHapticDeviceHandler();
	}

	chai3d::cGenericHapticDevicePtr device1;
	chai3d::cGenericHapticDevicePtr device2;

	handler.getDevice(device1, 0);
	p1Haptics = new HapticsController(device1);

	handler.getDevice(device2, 1);
	p2Haptics = new HapticsController(device2);

	p1Haptics->setPartner(p2Haptics);
	p2Haptics->setPartner(p1Haptics);
}

// TODO
void Program::setUpViews() {

	GLFWmonitor** monitors = glfwGetMonitors(&numMonitors);

	if (numMonitors < 2) {
		std::cout << "Warning: Game best played on two monitors" << std::endl;
		fullscreen = false;
		p2View = new PlayerView(*p2Haptics, monitors[0], fullscreen);
	}
	else {
		p2View = new PlayerView(*p2Haptics, monitors[1], fullscreen);
	}
	p1View = new PlayerView(*p1Haptics, monitors[0], fullscreen);
}

// Starts the program
void Program::start() {

	// Start the haptics thread and then enter the graphics loop
	next = p1Haptics;
	hapticsThread1.start(startNextHapticsLoop, chai3d::CTHREAD_PRIORITY_HAPTICS);

	while (next != nullptr);

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

// Called to close and clean up program
void Program::closeHaptics() {

	// Wait for haptics loop to terminate
	p1Haptics->stop();
	p2Haptics->stop();
	while (!p1Haptics->isFinished() && !p2Haptics->isFinished()) {
		chai3d::cSleepMs(100);
	}
}

// Starts the haptics controller loop of "next"
void Program::startNextHapticsLoop() {
	HapticsController* thread = next;
	next = nullptr;
	thread->start();
}

// Callback to print GLFW errors
void Program::errorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}

// Toggles fullscreen mode on and off
void Program::toggleFullscreen() {
	
	if (numMonitors < 2) {
		std::cout << "Cannot go fullscreen with only 1 monitor" << std::endl;
		return;
	}

	fullscreen = !fullscreen;
	p1View->setFullscreen(fullscreen);
	p2View->setFullscreen(fullscreen);
}

// Swap which device is associated with each view
void Program::swapDevices() {

}