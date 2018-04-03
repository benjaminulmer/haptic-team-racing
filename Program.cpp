#include "Program.h"

#include <string>

#include "InputHandler.h"
#include "ContentReadWrite.h"
#include "WorldLoader.h"

HapticsController* volatile Program::next;

// Default constructor for program
Program::Program() {

	fullscreen = false;
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
	world = new chai3d::cWorld();
	setUpHapticDevices();
	setUpViews();

	p1Haptics->setupTool(world, p1View->getCamera());
	p2Haptics->setupTool(world, p2View->getCamera());

	// Add cursors to the view
	p1View->addChild(p1Haptics->getCursor());
	p1View->addChild(p2Haptics->getCursorCopy());

	p2View->addChild(p2Haptics->getCursor());
	p2View->addChild(p1Haptics->getCursorCopy());

	// Temporarily load level here
	WorldLoader::loadWorld(ContentReadWrite::readJSON("worlds/cylinderWorld.json"), entities);

	for (const Entity* e : entities) {

		world->addChild(e->mesh);

		if (e->getView() == View::P1) {
			p1View->addChild(e->mesh);
		}
		else if (e->getView() == View::P2){
			p2View->addChild(e->mesh);
		}
		else { // both
			p1View->addChild(e->mesh);
			p2View->addChild(e->mesh);
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
	p1Haptics = new HapticsController(device1, entities);

	handler.getDevice(device2, 1);
	p2Haptics = new HapticsController(device2, entities);

	p1Haptics->setPartner(p2Haptics);
	p2Haptics->setPartner(p1Haptics);
}

// Sets up a view for each player. If more than one monitor connected each view is on a seperate monitor
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

	// TODO move game logic somewhere else
	chai3d::cLabel* p1Label;
	p1Label = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI32());
	p1Label->m_fontColor.setWhite();
	p1View->getCamera()->m_frontLayer->addChild(p1Label);

	chai3d::cLabel* p2Label;
	p2Label = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI32());
	p2Label->m_fontColor.setWhite();
	p2View->getCamera()->m_frontLayer->addChild(p2Label);

	chai3d::cPrecisionClock clock;
	clock.start();

	bool win = false;

	while (!p1View->shouldClose() && !p2View->shouldClose()) {

		glfwPollEvents();

		if (p1Haptics->getWorldPosition().x() < -0.5 && p2Haptics->getWorldPosition().x() < -0.5) {
			win = true;
		}

		if (!win) {
			p1Label->setText(chai3d::cStr(clock.getCurrentTimeSeconds(), 1) + "s");
			p2Label->setText(chai3d::cStr(clock.getCurrentTimeSeconds(), 1) + "s");
		}
		else {
			p1Label->setText("You're a winner!");
			p2Label->setText("You're a winner!");
		}
		p1Label->setLocalPos((int)(0.5 * (p1View->getWidth() - p1Label->getWidth())), p1View->getHeight() - 45);
		p2Label->setLocalPos((int)(0.5 * (p2View->getWidth() - p2Label->getWidth())), p2View->getHeight() - 45);

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