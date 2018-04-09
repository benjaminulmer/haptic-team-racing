#pragma once

#include "chai3d.h"
#include <GLFW/glfw3.h>

class UserInterface {
public:
	UserInterface(chai3d::cWorld* screen, GLFWwindow* window) : screen(screen), window(window) {};

	void setupMenu();
	void setupGameUI();

	void updateMenuLabels(int levelSelect);
	
	void addElement();

private:
	chai3d::cWorld* screen;
	GLFWwindow* window;

	// Game UI items
	chai3d::cLabel* p1Label;
	chai3d::cLabel* p2Label;

	// Menu items
	chai3d::cBitmap* bg;
	chai3d::cBitmap* selectPanel;
	chai3d::cLabel* startLabel;
	chai3d::cBitmap* logo;
	chai3d::cBitmap* level1;
	chai3d::cLabel* level1Label;
	chai3d::cBitmap* level2;
	chai3d::cLabel* level2Label;
};