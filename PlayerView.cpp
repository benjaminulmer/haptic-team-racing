#include "PlayerView.h"

#include <iostream>

#include "InputHandler.h"

// Creates a GLFW window for the player view
PlayerView::PlayerView(const HapticsController& controller, GLFWmonitor* monitor, bool fullscreen) : controller(controller) {

	// Get window width and height
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);


	// Set window hints and OpenGL version
	glfwWindowHint(GLFW_AUTO_ICONIFY, false);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Create window
	if (fullscreen) {
		window = glfwCreateWindow(mode->width, mode->height, "CHAI3D", monitor, NULL);
	}
	else {
		int w = 0.8 * mode->height;
		int h = 0.5 * mode->height;
		int x = 0.5 * (mode->width - w);
		int y = 0.5 * (mode->height - h);

		if (monitor != glfwGetPrimaryMonitor()) {
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			x += mode->width;
		}

		window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
		glfwSetWindowPos(window, x, y);
	}


	if (!window) {
		std::cerr << "failed to create GLFW window" << std::endl;
		system("pause");
		glfwTerminate();
		exit(-1);
	}

	// Set window properties
	glfwGetWindowSize(window, &width, &height);
	//glfwSetWindowPos(window, 0, 0);
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1); - not working right now because there are two windows

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
	light = new chai3d::cSpotLight(world);
	light->setEnabled(true);
	light->setLocalPos(0.7, 0.3, 1.0);

	// define the direction of the light beam
	light->setDir(-0.5, -0.2, -0.8);
	// enable this light source to generate shadows
	light->setShadowMapEnabled(true);
	// set the resolution of the shadow map
	light->m_shadowMap->setQualityVeryHigh();

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

	chai3d::cVector3d pos = controller.getWorldPosition();

	// Update haptic and graphic rate data
	labelRates->setText(chai3d::cStr(graphicsFreq.getFrequency(), 0) + " Hz / " +
						chai3d::cStr(controller.getFrequency(), 0) + " Hz / " +
						"pos: " + chai3d::cStr(pos.x()) + " " + chai3d::cStr(pos.y()) + " " + chai3d::cStr(pos.z()));
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
GLFWwindow * PlayerView::getWindow() const {
	return window;;
}

void PlayerView::addChild(chai3d::cMultiMesh* mesh) {
	mesh->m_material->setBlue();
	world->addChild(mesh);
}