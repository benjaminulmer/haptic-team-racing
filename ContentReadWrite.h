#pragma once

#include <rapidjson/document.h>

#include <string>

class ContentReadWrite {

public:
	static rapidjson::Document readJSON(std::string path);
};

