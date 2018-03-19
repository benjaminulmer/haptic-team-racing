#include "Program.h"

#include <iostream>

#include "InputHandler.h"

Program* Program::p;

// Default constructor for program
Program::Program() :
	window(nullptr),
	width(0),
	height(0),
	simulationRunning(false),
	simulationFinished(false),
	fullscreen(false),
	mirrored(false)
{
	p = this;
	InputHandler::setUp(this);
	printControls();
	setUpWindow();

	// Initialize GLEW library
	if (glewInit() != GLEW_OK) {
		std::cout << "failed to initialize GLEW library" << std::endl;
		system("pause");
		glfwTerminate();
		exit(-1);
	}

	setUpWorld();
	setUpHapticDevice();
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
	std::cout << "[Falcon Button 0] - Change scene" << std::endl;
	std::cout << "[Falcon Button 1] - Restart game (when in scene 4)" << std::endl;
	std::cout << std::endl << std::endl;
}

// Creates and sets up GLFW window
void Program::setUpWindow() {

	// Initialize GLFW library
	if (!glfwInit()) {
		std::cerr << "failed GLFW initialization" << std::endl;
		system("pause");
		exit(-1);
	}

	// Get window width and height
	glfwSetErrorCallback(errorCallback);
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

// Initializes geometry of the world
void Program::setUpWorld() {

	world = new chai3d::cWorld();
	cursor = new chai3d::cShapeSphere(0.0075);

	// Set up camera
	camera = new chai3d::cCamera(world);
	camera->set(chai3d::cVector3d (0.25, 0.0, 0.05),    // camera position (eye)
			    chai3d::cVector3d (0.0, 0.0, 0.0),    // look at position (target)
			    chai3d::cVector3d (0.0, 0.0, 1.0));   // direction of the (up) vector
	camera->setClippingPlanes(0.01, 10.0);
	camera->setMirrorVertical(mirrored);

	// Set up light
	light = new chai3d::cDirectionalLight(world);
	light->setEnabled(true);
	light->setDir(-1.0, 0.0, 0.0);

	// Set up label
	labelRates = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI20());
	labelRates->m_fontColor.setWhite();
	camera->m_frontLayer->addChild(labelRates);

	// Add objects to world
	world->m_backgroundColor.setBlack();
	world->addChild(camera);
	world->addChild(light);
	world->addChild(cursor);
}

// Initializes haptics device
void Program::setUpHapticDevice() {

	// Get, open, and calibrate device
	handler.getDevice(hapticDevice, 0);
	hapticDevice->open();
	hapticDevice->calibrate();

	// Get info about the current haptic device
	chai3d::cHapticDeviceInfo info = hapticDevice->getSpecifications();

	// Display a reference frame if haptic device supports orientations
	if (info.m_sensedRotation == true) {

		cursor->setShowFrame(true);
		cursor->setFrameSize(0.05);
	}

	// If the device has a gripper, enable the gripper to simulate a user switch
	hapticDevice->setEnableGripperUserSwitch(true);
}

// Starts the program
void Program::start() {

	// Start the haptics thread and then enter the graphics loop
	hapticsThread.start(hapticsLoop, chai3d::CTHREAD_PRIORITY_HAPTICS);
	mainLoop();
}

// Main loop for running graphics and other non-haptics work
void Program::mainLoop() {

	while (!glfwWindowShouldClose(window)) {

		// Update graphics and window
		updateGraphics();

		glfwSwapBuffers(window);
		glfwPollEvents();
		freqCounterGraphics.signal(1);
	}

	// Clean up
	close();
	glfwDestroyWindow(window);
	glfwTerminate();
}

// Updates graphics of the program
void Program::updateGraphics() {

	// Update haptic and graphic rate data
	labelRates->setText(chai3d::cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
	                    chai3d::cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");
	labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);

	// Render world
	world->updateShadowMaps(false, mirrored);
	camera->renderView(width, height);
	glFinish();

	// Check for any OpenGL errors
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "Error:  %s\n" << gluErrorString(err);
	}
}

// Main loop for running haptics
void Program::hapticsLoop() {

	// Simulation is starting
	p->simulationRunning = true;
	p->simulationFinished = false;

	bool button0Hold = false;
	bool button1Hold = false;
	while(p->simulationRunning) {

		// Read pointer position and orientation (if exists)
		chai3d::cVector3d position;
		chai3d::cMatrix3d rotation;
		p->hapticDevice->getPosition(position);
		p->hapticDevice->getRotation(rotation);

		// Read status of buttons
		bool pressed;
		p->hapticDevice->getUserSwitch(0, pressed);

		if (pressed && !button0Hold) {
			// button pressed
		}
		button0Hold = pressed;

		p->hapticDevice->getUserSwitch(1, pressed);
		if (pressed && !button0Hold) {
			// button pressed
		}
		button1Hold = pressed;

		// Update position and orienation of cursor
		p->cursor->setLocalPos(position);
		p->cursor->setLocalRot(rotation);

		// Calculate forces on cursor
		chai3d::cVector3d force(0.0, 0.0, 0.0);

		// CALCULATE FORCES

		chai3d::cVector3d torque(0.0, 0.0, 0.0);
		double gripperForce = 0.0;

		// Send computed force, torque, and gripper force to haptic device
		p->hapticDevice->setForceAndTorqueAndGripperForce(force, torque, gripperForce);
		p->freqCounterHaptics.signal(1);
	}

	// Exit haptics thread
	p->simulationFinished = true;
}

// Called to close and clean up program
void Program::close() {

	// Wait for haptics loop to terminate
	p->simulationRunning = false;
	while (!p->simulationFinished) {
		chai3d::cSleepMs(100);
	}

	// Clean up
	p->hapticDevice->close();
}

// Sets window size to new parameters
void Program::setWindowSize(int width, int height) {
	this->width = width;
	this->height = height;
}

// Toggles fullscreen mode on and off
void Program::toggleFullscreen() {
	
	fullscreen = !fullscreen;

	// Get info about monitor
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Set fullscreen or window mode
	if (fullscreen) {
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		glfwSwapInterval(1);
	}
	else {
		int w = 0.8 * mode->height;
		int h = 0.5 * mode->height;
		int x = 0.5 * (mode->width - w);
		int y = 0.5 * (mode->height - h);
		glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
		glfwSwapInterval(1);
	}
}

// Callback to print GLFW errors
void Program::errorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}