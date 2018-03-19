#include "PlayerView.h"

#include <iostream>

#include "InputHandler.h"

// Creates a GLFW window for the player view
PlayerView::PlayerView(const HapticsController& controller) : controller(controller) {

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

	setUpWorld();
}

// Closes the GLFW window
PlayerView::~PlayerView() {
	glfwDestroyWindow(window);
}

// Initializes geometry of the world
void PlayerView::setUpWorld() {

	world = new chai3d::cWorld();

	// Set up camera
	camera = new chai3d::cCamera(world);
	camera->set(chai3d::cVector3d(0.25, 0.0, 0.05),   // camera position (eye)
		chai3d::cVector3d(0.0, 0.0, 0.0),    // look at position (target)
		chai3d::cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector
	camera->setClippingPlanes(0.01, 10.0);

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
}

// Render the current view
void PlayerView::render() {

	glfwMakeContextCurrent(window);

	// Update haptic and graphic rate data
	labelRates->setText(chai3d::cStr(graphicsFreq.getFrequency(), 0) + " Hz / " +
	                    chai3d::cStr(controller.getFrequency(), 0) + " Hz");
	labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);

	// Render world
	world->updateShadowMaps();
	camera->renderView(width, height);
	glFinish();

	// Check for any OpenGL errors
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "Error:  %s\n" << gluErrorString(err);
	}


	graphicsFreq.signal(1);
	glfwSwapBuffers(window);
}

// Returns if the GLFW window should close
bool PlayerView::shouldClose() const{
	return glfwWindowShouldClose(window);
}

// Returns constant pointer to the GLFW window of the view
const GLFWwindow * PlayerView::getWindow() const {
	return window;;
}