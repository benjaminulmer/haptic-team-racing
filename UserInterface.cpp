#include "UserInterface.h"

void UserInterface::setupMenu() {
	bg = new chai3d::cBitmap();
	bg->loadFromFile("textures/bg.png");
	screen->addChild(bg);

	selectPanel = new chai3d::cBitmap();
	selectPanel->loadFromFile("textures/selection.png");
	screen->addChild(selectPanel);

	startLabel = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI32());
	startLabel->m_fontColor.setBlueDarkTurquoise();
	startLabel->setFontScale(2.0);
	screen->addChild(startLabel);
	startLabel->setText("Use arrow keys and press ENTER to select a level!");

	logo = new chai3d::cBitmap();
	logo->loadFromFile("textures/logo.png");
	screen->addChild(logo);
	logo->setZoom(0.5, 0.5);

	level1 = new chai3d::cBitmap();
	level1->loadFromFile("textures/level.bmp");
	screen->addChild(level1);
	level1->setZoom(0.5, 0.5);

	level1Label = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI32());
	level1Label->m_fontColor.setBlueDark();
	screen->addChild(level1Label);
	level1Label->setText("Obstacles level (easy)");

	level2 = new chai3d::cBitmap();
	level2->loadFromFile("textures/technotube.png");
	screen->addChild(level2);
	level2->setZoom(0.5, 0.5);

	level2Label = new chai3d::cLabel(chai3d::NEW_CFONTCALIBRI32());
	level2Label->m_fontColor.setBlueDark();
	screen->addChild(level2Label);
	level2Label->setText("Techno Tube (Hard)");
}

void UserInterface::updateMenuLabels(int levelSelect) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	startLabel->setLocalPos((int)(0.5 * (width - startLabel->getWidth())), 100);
	logo->setLocalPos((int)(0.5 * (width - logo->getWidth())), (height - logo->getHeight()));

	level1->setLocalPos((int)(0.5 * (width - 2.0 * level1->getWidth()) - 10.0), (height - level1->getHeight() - logo->getHeight() - 50));
	level1Label->setLocalPos((int)(0.5 * (width - 2.0 * level1->getWidth() + level1Label->getWidth())), (height - level1->getHeight() - logo->getHeight() - level1Label->getHeight() - 60));

	level2->setLocalPos((int)(0.5 * (width) + 10.0), (height - level2->getHeight() - logo->getHeight() - 50));
	level2Label->setLocalPos((int)(0.5 * (width + level2->getWidth() - level2Label->getWidth())), (height - level2->getHeight() - logo->getHeight() - level2Label->getHeight() - 60));

	bg->setLocalPos(chai3d::cVector3d(0.0, 0.0, 0.0));

	if (levelSelect == 0) {
		selectPanel->setLocalPos(level1->getLocalPos() - chai3d::cVector3d(10.0, 10.0, 0.0));
	}
	else selectPanel->setLocalPos(level2->getLocalPos() - chai3d::cVector3d(10.0, 10.0, 0.0));
}

void UserInterface::setupGameUI() {

}

void UserInterface::addLabel(chai3d::cLabel* label) {
	screen->addChild(label);
	infoLabel = label;
}

void UserInterface::endGame(bool hasWin) {
	endScreen = new chai3d::cBitmap();
	std::string filename;

	if (hasWin) {
		filename = "textures/win.png";
	}
	else {
		filename = "textures/lose.png";
	}

	endScreen->loadFromFile(filename);
	screen->addChild(endScreen);
}

void UserInterface::reset() {
	screen->removeChild(endScreen);
	//delete endScreen;
}

void UserInterface::updateEndScreen() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	endScreen->setLocalPos((int)(0.5 * (width - endScreen->getWidth())), (int)(0.5 * (height - endScreen->getHeight())));
}

void UserInterface::updateInfoLabel() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	infoLabel->setLocalPos((int)(0.5 * (width - infoLabel->getWidth())), height - 45);
}

void UserInterface::setInfoLabelText(std::string text) {
	infoLabel->setText(text);
}