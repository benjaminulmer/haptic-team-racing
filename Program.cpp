#include "Program.h"

#include <string>

#include "InputHandler.h"
#include "ContentReadWrite.h"
#include "WorldLoader.h"
#include "Hazard.h"
#include "Collectible.h"

HapticsController* volatile Program::next;

// Default constructor for program
Program::Program() : state(State::DEFAULT), inMenu(true), levelSelect(0) {

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

	// Add cursors to the view
	p1View->addChild(p1Haptics->getCursor());
	p1View->addChild(p2Haptics->getCursorCopy());

	p2View->addChild(p2Haptics->getCursor());
	p2View->addChild(p1Haptics->getCursorCopy());

	// Initialize GLEW library
	if (glewInit() != GLEW_OK) {
		std::cout << "failed to initialize GLEW library" << std::endl;
		system("pause");
		glfwTerminate();
		exit(-1);
	}

	setUpMenu();
	menuLoop();

	loadLevel();
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

	p1Haptics->setupTool(world);
	p2Haptics->setupTool(world);

	// Connect signals for haptic events
	p1Haptics->destroyEntity.connect_member(this, &Program::destroyEntity);
	p2Haptics->destroyEntity.connect_member(this, &Program::destroyEntity);

	p1Haptics->springBroken.connect_member(this, &Program::loseGame);
}

// Sets up a view for each player. If more than one monitor connected each view is on a seperate monitor
void Program::setUpViews() {

	monitors = glfwGetMonitors(&numMonitors);

	if (numMonitors < 2) {
		std::cout << "Warning: Game best played on two monitors" << std::endl;
		fullscreen = false;
		p1View = new PlayerView(*p2Haptics, monitors[0], fullscreen);
	}
	else {
		p1View = new PlayerView(*p2Haptics, monitors[1], fullscreen);
	}
	p2View = new PlayerView(*p1Haptics, monitors[0], fullscreen);
}

// Load level from specified file
void Program::loadLevel() {

	for (Entity* e : entities) {
		world->removeChild(e->mesh);
		p1View->getWorld()->removeChild(e->mesh);
		p2View->getWorld()->removeChild(e->mesh);
		delete e;
	}
	entities.clear();

	maxTime = WorldLoader::loadWorld(ContentReadWrite::readJSON(selectedLevel), entities);

	for (Entity* e : entities) {

		// Connect entity signals to the game slots
		Type t = e->getType();
		if (t == Type::HAZARD) {
			Hazard* h = (Hazard*)e;
			h->hitHazard.connect_member(this, &Program::loseGame);
		}
		else if (t == Type::COLLECTIBLE) {
			Collectible* c = (Collectible*)e;
			c->pickUpCollectible.connect_member(this, &Program::addTime);
		}

		// Add entity to haptic world
		world->addChild(e->mesh);

		// Add enitty to appropriate view world
		if (e->getView() == View::P1) {
			p1View->addChild(e->mesh);
		}
		else if (e->getView() == View::P2) {
			p2View->addChild(e->mesh);
		}
		else if (e->getView() == View::BOTH) { // both
			p1View->addChild(e->mesh);
			p2View->addChild(e->mesh);
		}
	}
}

// Starts the program
void Program::start() {

	startHaptics();
	mainLoop();
}

// Main loop for running graphics and other non-haptics work
void Program::mainLoop() {

	chai3d::cLabel* p1 = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI32());
	p1->m_fontColor.setWhite();
	p1View->getUI()->addLabel(p1);

	chai3d::cLabel* p2 = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI32());
	p2->m_fontColor.setWhite();
	p2View->getUI()->addLabel(p2);

	p1View->setFullscreen(fullscreen);
	p2View->setFullscreen(fullscreen);

	clock.start();

	state = State::RUNNING;

	while (!p1View->shouldClose() && !p2View->shouldClose()) {

		glfwPollEvents();
		double timeS = clock.getCurrentTimeSeconds();

		if (state == State::RUNNING) {

			if (timeS >= maxTime) {
				loseGame();
			}
			else if (p1Haptics->getWorldPosition().x() < -0.5 && p2Haptics->getWorldPosition().x() < -0.5) {
				winGame();
			}

			p1View->getUI()->setInfoLabelText(chai3d::cStr(maxTime - timeS, 1) + "s");
			p2View->getUI()->setInfoLabelText(chai3d::cStr(maxTime - timeS, 1) + "s");
		}
		else if (state == State::WIN){
			p1View->getUI()->endGame(true);
			p2View->getUI()->endGame(true);
			state = State::END;
			closeHaptics();
		}
		else if (state == State::LOSE) {
			p1View->getUI()->endGame(false);
			p2View->getUI()->endGame(false);
			state = State::END;
			closeHaptics();
		}
		else if (state == State::END) {
			p1View->getUI()->updateEndScreen();
			p2View->getUI()->updateEndScreen();
		}

		p1View->getUI()->updateInfoLabel();
		p2View->getUI()->updateInfoLabel();

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

// Called when a lose trigger is activated
void Program::loseGame() {
	state = State::LOSE;
}

// Called when the win trigger is activated
void Program::winGame() {
	state = State::WIN;
}

void Program::addTime(double amount) {
	clock.reset(clock.getCurrentTimeSeconds() - amount);
}

// Removes entity from each view world, haptic world, and entity list
void Program::destroyEntity(Entity* entity) {

	world->removeChild(entity->mesh);
	p1View->getWorld()->removeChild(entity->mesh);
	p2View->getWorld()->removeChild(entity->mesh);

	std::vector<Entity*>::iterator it;
	for (it = entities.begin(); it < entities.end(); it++) {

		if ((*it) == entity) {
			entities.erase(it);
			break;
		}
	}
	delete entity;
}

// Called to start haptic interaction
void Program::startHaptics() {

	next = p1Haptics;
	hapticsThread1.start(startNextHapticsLoop, chai3d::CTHREAD_PRIORITY_HAPTICS);

	while (next != nullptr);

	next = p2Haptics;
	hapticsThread2.start(startNextHapticsLoop, chai3d::CTHREAD_PRIORITY_HAPTICS);
}

// Called to close and clean up program
void Program::closeHaptics() {

	// Wait for haptics loops to terminate
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

	if (inMenu) {
		menuView->setFullscreen(fullscreen);
	}
	else {
		p1View->setFullscreen(fullscreen);
		p2View->setFullscreen(fullscreen);
	}
}

// Swap which device is associated with each view
void Program::swapDevices() {
}

void Program::moveCamera(double dir) {
	chai3d::cCamera* cam1 = p1View->getCamera();
	chai3d::cCamera* cam2 = p2View->getCamera();

	chai3d::cVector3d disp;

	if (dir > 0.0) {
		disp = chai3d::cVector3d(1.0, 0.0, 0.0);
	}
	else disp = chai3d::cVector3d(-1.0, 0.0, 0.0);

	cam1->setLocalPos(0.005 * disp + cam1->getLocalPos());
	cam2->setLocalPos(0.005 * disp + cam2->getLocalPos());

}

void Program::menuLoop() {
	menuView->getUI()->setupMenu();

	while (inMenu && !menuView->shouldClose()) {
		glfwPollEvents();

		menuView->getUI()->updateMenuLabels(levelSelect);
		menuView->render();
	}

	if (levelSelect == 0) {
		selectedLevel = "worlds/obstaclesWorld.json";
		p1Haptics->setPosiiton(chai3d::cVector3d(0.45, 0.01, 0.0));
		p2Haptics->setPosiiton(chai3d::cVector3d(0.45, -0.01, 0.0));
	}
	else {
		selectedLevel = "worlds/cylinderWorld.json";
		p1Haptics->setPosiiton(chai3d::cVector3d(0.5, 0.01, 0.0));
		p2Haptics->setPosiiton(chai3d::cVector3d(0.5, -0.01, 0.0));
	}
	delete menuView;
}

void Program::setUpMenu() {
	menuView = new PlayerView(*p1Haptics, monitors[0], fullscreen, true);
	menuView->getWorld()->m_backgroundColor.setYellowGold();
}

void Program::toggleLevelSelect() {
	if (levelSelect == 0) {
		levelSelect = 1;
	}
	else levelSelect = 0;
}

void Program::restartGame() {

	p1Haptics->setPosiiton(chai3d::cVector3d(0.55, 0.01, 0.0));
	p2Haptics->setPosiiton(chai3d::cVector3d(0.55, -0.01, 0.0));

	p1Haptics->reset();
	p2Haptics->reset();

	p1View->getUI()->reset();
	p2View->getUI()->reset();

	state = State::DEFAULT;
	inMenu = true;
	setUpMenu();
	menuLoop();
	loadLevel();

	startHaptics();

	clock.reset(0.0);
	state = State::RUNNING;
}