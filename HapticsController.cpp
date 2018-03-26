#include "HapticsController.h"

// Creates a controller for the provided haptic device
HapticsController::HapticsController(chai3d::cGenericHapticDevicePtr device) : device(device) {
	
	running = false;
	finished = false;

	device->open();
	device->calibrate();
}

// Closes device controller
HapticsController::~HapticsController() {
	device->close();
}

// Sets the partner for the controller
void HapticsController::setPartner(const HapticsController* partner) {
	this->partner = partner;
}

void HapticsController::setupTool(chai3d::cWorld* w, chai3d::cCamera* c) {

	camera = c;
	world = w;

	tool = new chai3d::cToolCursor(world);
	tool->setHapticDevice(device);
	tool->setRadius(0.01); // 1 cm
	tool->m_hapticPoint->m_sphereProxy->m_material->setBlue();
	tool->start();
	world->addChild(tool);
};

// Starts the haptics loop
void HapticsController::start() {

	// Simulation is starting
	running = true;

	bool button0Hold = false;
	while (running) {

		// Read pointer position and orientation (if exists)
		device->getPosition(curPos);

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
		chai3d::cVector3d partnerPos = partner->getWorldPosition();//getPosition();
		chai3d::cVector3d dir = partnerPos - getWorldPosition();//curPos;
		double dist = dir.length();
		dir.normalize();

		constexpr double rest = 0.00;
		constexpr double k = 300.0;

		if (dist >= rest) {
			force += dir * (dist - rest) * k;
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

		// Send computed force, torque, and gripper force to haptic device
		//device->setForceAndTorqueAndGripperForce(force, torque, gripperForce);

		hapticFreq.signal(1);
	}

	// Exit haptics thread
	finished = true;
}

// We only want rate control along the x-axis
void HapticsController::checkRateControl() {

	if ((curPos.x() < -0.03) || (curPos.x() > 0.035)) {
		double s = 0.002;

		// Move tool
		chai3d::cVector3d disp = tool->getLocalPos() + (s * curPos);
		tool->setLocalPos(chai3d::cVector3d(disp.x(), tool->getLocalPos().y(), tool->getLocalPos().z()));

		// Move camera
		chai3d::cVector3d cPos = camera->getLocalPos();
		cPos = cPos + (s * curPos);
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

// Returns current position of device
chai3d::cVector3d HapticsController::getPosition() const {
	return curPos;
}

// Returns current rotation of device
chai3d::cMatrix3d HapticsController::getRotation() const {
	return curRot;
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