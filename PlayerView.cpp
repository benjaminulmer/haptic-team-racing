#include "PlayerView.h"

#include "InputHandler.h"

std::map<GLFWwindow*, PlayerView*> PlayerView::windowToView;

// Creates a GLFW window for the player view
PlayerView::PlayerView(const HapticsController& controller, GLFWmonitor* monitor, bool fullscreen, bool isMenu) : controller(controller), monitor(monitor), isMenu(isMenu) {

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

	// Add window view pair to mapping
	windowToView[window] = this;

	// Set window properties
	glfwGetWindowSize(window, &width, &height);
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1); - TODO not working right now because there are two windows

	// Set callback functions
	glfwSetKeyCallback(window, InputHandler::keyCallback);
	glfwSetWindowSizeCallback(window, PlayerView::windowSizeCallback);

	setUpWorld();
}

// Closes the GLFW window
PlayerView::~PlayerView() {
	glfwDestroyWindow(window);
}

// Initializes geometry of the world
void PlayerView::setUpWorld() {

	world = new chai3d::cWorld();
	world->setGhostEnabled(true);

	// Set up camera
	camera = new chai3d::cCamera(world);
	camera->set(chai3d::cVector3d(0.1, 0.0, 0.0),  // camera position (eye)
		        chai3d::cVector3d(0.0, 0.0, 0.0),    // look at position (target)
		        chai3d::cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector
	camera->setClippingPlanes(0.061, 10.0);

	camera->setUseMultipassTransparency(true);

	// Set up light
	light = new chai3d::cSpotLight(world);
	light->setEnabled(true);
	light->setLocalPos(0.6, -0.01, 0.01);

	// Define the direction of the light beam and turn on shadows
	light->setDir(-1.0, 0.25, 0.25);

	light->setShadowMapEnabled(true);
	light->m_shadowMap->setQualityVeryHigh();

	// Set up label
	labelRates = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI20());
	labelRates->m_fontColor.setWhite();
	camera->m_frontLayer->addChild(labelRates);

	// Add objects to world
	world->m_backgroundColor.setPurpleMediumSlateBlue();
	world->addChild(camera);
	world->addChild(light);

	chai3d::cDirectionalLight* ambientLight = new chai3d::cDirectionalLight(world);
	ambientLight->setEnabled(true);
	ambientLight->setDir(1.0, 0.1, 0.1);
	world->addChild(ambientLight);

	chai3d::cPositionalLight* ambientLight2 = new chai3d::cPositionalLight(world);
	ambientLight2->setEnabled(true);
	ambientLight2->setLocalPos(-0.55, 0.0, 0.0);
	world->addChild(ambientLight2);

	ui = new UserInterface(camera->m_frontLayer, window);
}

// Render the current view
void PlayerView::render() {

	glfwMakeContextCurrent(window);

	if (!isMenu) {
		chai3d::cVector3d pos = controller.getWorldPosition();
		chai3d::cVector3d newCamPos = pos;
		newCamPos.y(0.0); newCamPos.z(0.0);
		camera->setLocalPos(newCamPos + chai3d::cVector3d(0.1, 0.0, 0.0));

		light->setLocalPos(camera->getLocalPos() + chai3d::cVector3d(0.1, -0.01, 0.01));

		// Update haptic and graphic rate data
		labelRates->setText(chai3d::cStr(graphicsFreq.getFrequency(), 0) + " Hz / " +
			chai3d::cStr(controller.getFrequency(), 0) + " Hz / " +
			"camera pos: " + chai3d::cStr(camera->getLocalPos().x()) + " " + chai3d::cStr(camera->getLocalPos().y()) + " " + chai3d::cStr(camera->getLocalPos().z()));
		labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);

		// Render world
		world->updateShadowMaps();
	}
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
bool PlayerView::shouldClose() const {
	return glfwWindowShouldClose(window);
}

// Returns constant pointer to the GLFW window of the view
GLFWwindow * PlayerView::getWindow() const {
	return window;;
}

// Adds provided mesh to the world
void PlayerView::addChild(chai3d::cGenericObject* object) {
	world->addChild(object);
}

// Returns the veiw world
chai3d::cWorld * PlayerView::getWorld() {
	return world;
}

// Returns the camera of the view world
chai3d::cCamera * PlayerView::getCamera() {
	return camera;
}

// Returns current width of the window
int PlayerView::getWidth() const {
	return width;
}

// Returns current height of the window
int PlayerView::getHeight() const {
	return height;
}

// Sets fullscreen mode to the provided value
void PlayerView::setFullscreen(bool fullscreen) {

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Toggle state
	if (fullscreen) {
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else {
		int w = 0.8 * mode->height;
		int h = 0.5 * mode->height;
		int x = 0.5 * (mode->width - w);
		int y = 0.5 * (mode->height - h);

		if (monitor != glfwGetPrimaryMonitor()) {
			x += mode->width;
		}

		glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
	}
}

// Callback for updating the size of the window
void PlayerView::windowSizeCallback(GLFWwindow* window, int width, int height) {

	PlayerView* view = windowToView[window];
	view->width = width;
	view->height = height;

}