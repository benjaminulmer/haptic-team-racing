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
	avatarCopy = new chai3d::cShapeSphere(0.005);
	avatarCopy->m_material->setTransparencyLevel(0.3);
	avatarCopy->setUseTransparency(true);
	avatarCopy->m_material->setGreen();
};

// Starts the haptics loop
void HapticsController::start() {

	// Simulation is starting
	running = true;

	bool button0Hold = false;
	while (running) {

		// Read pointer position and orientation (if exists)
		device->getPosition(devicePos);

		// Read status of buttons
		bool pressed;
		device->getUserSwitch(0, pressed);

		if (pressed && !button0Hold) {
			// button pressed
		}
		button0Hold = pressed;

		// Calculate forces on cursor
		chai3d::cVector3d force(0.0, 0.0, 0.0);

		// *** Virtual spring connecting avatars
		chai3d::cVector3d partnerPos = partner->getWorldPosition();
		chai3d::cVector3d dir = partnerPos - getWorldPosition();
		double dist = dir.length();
		dir.normalize();

		if (dist >= Constants::springRest) {
			force += dir * (dist - Constants::springRest) * Constants::springK;
		}

		// *** End spring

		chai3d::cVector3d torque(0.0, 0.0, 0.0);
		double gripperForce = 0.0;


		/////////////////////////////////////////////////////////////////////
		// UPDATE 3D CURSOR MODEL
		/////////////////////////////////////////////////////////////////////

		// update position and orienation of cursor
		world->computeGlobalPositions();
		tool->updateFromDevice();

		chai3d::cVector3d newPos = getWorldPosition();
		for (const Entity* e : entities) {

			if (insideEntity.count(e) == 0) {
				insideEntity[e] = false;
			}

			chai3d::cCollisionRecorder r;
			chai3d::cCollisionSettings s;
			s.m_collisionRadius = Constants::cursorRadius;

			if (e->mesh->computeCollisionDetection(prevWorldPos, newPos, r, s)) {
				std::cout << "enter" << std::endl;
				insideEntity[e] = true;
			}
			else if (e->mesh->computeCollisionDetection(newPos, prevWorldPos, r, s)) {
				std::cout << "exit" << std::endl;
				insideEntity[e] = false;
			}

			if (insideEntity[e]) {
				std::cout << "i";
			}

		}
		prevWorldPos = getWorldPosition();

		// Update position of proxy as well
		avatarCopy->setLocalPos(getWorldPosition());

		/////////////////////////////////////////////////////////////////////
		// COMPUTE FORCES
		/////////////////////////////////////////////////////////////////////

		tool->computeInteractionForces();

		checkRateControl();

		/////////////////////////////////////////////////////////////////////
		// APPLY FORCES
		/////////////////////////////////////////////////////////////////////

		tool->addDeviceLocalForce(force);

		tool->applyToDevice();

		hapticFreq.signal(1);
	}

	// Exit haptics thread
	finished = true;
}

// We only want rate control along the x-axis
void HapticsController::checkRateControl() {

	if (abs(devicePos.x()) > 0.02) {
		double s = 0.003;

		// Move tool
		chai3d::cVector3d disp = tool->getLocalPos() + (s * devicePos);
		tool->setLocalPos(chai3d::cVector3d(disp.x(), tool->getLocalPos().y(), tool->getLocalPos().z()));

		// Move camera
		chai3d::cVector3d cPos = camera->getLocalPos();
		cPos = cPos + (s * devicePos);
		camera->setLocalPos(cPos.x(), camera->getLocalPos().y(), camera->getLocalPos().z());
	}
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
