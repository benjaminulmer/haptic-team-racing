#pragma once

#include <rapidjson/document.h>

#include <vector>

#include "Entity.h"

class WorldLoader {

public:
	static double loadWorld(rapidjson::Document d, std::vector<Entity*>& output);
};

