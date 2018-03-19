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

// Starts the haptics loop
void HapticsController::start() {

	// Simulation is starting
	running = true;

	bool button0Hold = false;
	while (running) {

		// Read pointer position and orientation (if exists)
		device->getPosition(curPos);
		device->getRotation(curRot);

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
		chai3d::cVector3d partnerPos = partner->getPosition();
		chai3d::cVector3d dir = partnerPos - curPos;
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

		// Send computed force, torque, and gripper force to haptic device
		device->setForceAndTorqueAndGripperForce(force, torque, gripperForce);
		hapticFreq.signal(1);
	}

	// Exit haptics thread
	finished = true;
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

// Returns current haptic frequency
double HapticsController::getFrequency() const {
	return hapticFreq.getFrequency();
}