#include "HapticsController.h"

#include "Constants.h"

// Creates a controller for the provided haptic device
HapticsController::HapticsController(chai3d::cGenericHapticDevicePtr device, const std::vector<Entity*>& entities) : device(device), entities(entities) {

	running = false;
	finished = false;

	device->open();
	device->calibrate();

	prevWorldPos.zero();
}

// Closes device controller
HapticsController::~HapticsController() {
	device->close();
}

// Sets the partner for the controller
void HapticsController::setPartner(const HapticsController* partner) {
	this->partner = partner;
}

// Sets up the haptic tool to interact with the given world
void HapticsController::setupTool(chai3d::cWorld* w, chai3d::cCamera* c) {

	camera = c;
	world = w;

	tool = new chai3d::cToolCursor(world);
	tool->setHapticDevice(device);
	tool->setRadius(Constants::cursorRadius);
	tool->m_hapticPoint->m_sphereProxy->m_material->setBlue();
	tool->start();
	world->addChild(tool);

	// Create a renderable copy of the tool (for other player's view)
	avatarCopy = new chai3d::cShapeSphere(Constants::cursorRadius);
	avatarCopy->m_material->setTransparencyLevel(0.3);
	avatarCopy->setUseTransparency(true);
	avatarCopy->m_material->setGreen();
};

// Starts the haptics loop
void HapticsController::start() {

	running = true;

	bool button0Hold = false;
	while (running) {

		// Read status of buttons
		bool pressed;
		device->getUserSwitch(0, pressed);

		if (pressed && !button0Hold) {
			// button pressed
		}
		button0Hold = pressed;

		// Update positions
		device->getPosition(devicePos);
		world->computeGlobalPositions();
		tool->updateFromDevice();
		avatarCopy->setLocalPos(prevWorldPos);

		// Perform interactions and calculate forces
		tool->computeInteractionForces();
		performRateControl();
		applySpringForce();
		performEntityInteraction();

		// Apply forces to tool and signal frequency counter
		tool->applyToDevice();
		hapticFreq.signal(1);
	}

	// Exit haptics thread
	finished = true;
}

// Performs interaction between cursor and entities in the world
void HapticsController::performEntityInteraction() {

	chai3d::cVector3d force(0.0, 0.0, 0.0);
	chai3d::cVector3d newPos = getWorldPosition();

	for (Entity* e : entities) {

		if (insideEntity.count(e) == 0) {
			insideEntity[e] = false;
		}

		chai3d::cCollisionRecorder r;
		chai3d::cCollisionSettings s;
		s.m_collisionRadius = Constants::cursorRadius;

		// Test if cursor entered entity
		if (e->mesh->computeCollisionDetection(prevWorldPos, newPos, r, s)) {
			insideEntity[e] = true;
		}
		// Test if cursor exitted entity
		else if (e->mesh->computeCollisionDetection(newPos, prevWorldPos, r, s)) {
			insideEntity[e] = false;
		}

		if (insideEntity[e]) {
			force += e->interact(tool);
		}
	}
	prevWorldPos = getWorldPosition();

	tool->addDeviceLocalForce(force);
}

// Computes and applies spring foce to tool
void HapticsController::applySpringForce() {

	chai3d::cVector3d force(0.0, 0.0, 0.0);
	chai3d::cVector3d partnerPos = partner->getWorldPosition();
	chai3d::cVector3d dir = partnerPos - getWorldPosition();
	double dist = dir.length();
	dir.normalize();

	// Calculate spring force only if spring is elongated
	if (dist >= Constants::springRest) {
		force = dir * (dist - Constants::springRest) * Constants::springK;
	}
	tool->addDeviceLocalForce(force);
}

// Updates tool and camera position based on rate control rules
void HapticsController::performRateControl() {

	chai3d::cVector3d disp(0.0, 0.0, 0.0);
	chai3d::cVector3d xPos(devicePos.x(), 0.0, 0.0);

	if (devicePos.x() > Constants::rateZone) {

		disp = xPos - chai3d::cVector3d(Constants::rateZone, 0.0, 0.0);
		tool->setLocalPos(Constants::rateScale * disp + tool->getLocalPos());

		// Velocity setting not great
		tool->setDeviceLocalLinVel((Constants::rateScale * disp) / 0.001 + tool->getDeviceLocalLinVel());
		camera->setLocalPos(Constants::rateScale * disp + camera->getLocalPos());
	}
	else if (devicePos.x() < -Constants::rateZone) {

		disp = xPos - chai3d::cVector3d(-Constants::rateZone, 0.0, 0.0);
		tool->setLocalPos(Constants::rateScale * disp + tool->getLocalPos());

		// Velocity setting not great
		tool->setDeviceLocalLinVel((Constants::rateScale * disp) / 0.001 + tool->getDeviceLocalLinVel());
		camera->setLocalPos(Constants::rateScale * disp + camera->getLocalPos());
	}
	tool->addDeviceLocalForce(-Constants::rateFeedback * disp);
}

// Tells the haptics thread to stop running
void HapticsController::stop() {
	running = false;
}

// Returns if haptics thread has finished running
bool HapticsController::isFinished() const {
	return finished;
}

// Returns the position of the proxy in world coordinates
chai3d::cVector3d HapticsController::getWorldPosition() const {

	chai3d::cTransform t = tool->getLocalTransform();
	chai3d::cVector3d p = tool->m_hapticPoint->m_sphereProxy->getLocalPos();

	return t * p;
}

// Returns current haptic frequency
double HapticsController::getFrequency() const {
	return hapticFreq.getFrequency();
}

// Returns a pointer to the haptic tool cursor
chai3d::cToolCursor * HapticsController::getCursor() {
	return tool;
}

// Returns a pointer to a renderable copy of the cursor
chai3d::cShapeSphere * HapticsController::getCursorCopy() {
	return avatarCopy;
}
