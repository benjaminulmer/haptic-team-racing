#include "WorldLoader.h"

#include <string>

// Returns a vector of all entities from a world file
void WorldLoader::loadWorld(rapidjson::Document d, std::vector<Entity>& output) {

	rapidjson::Value& entities = d["entities"];
	for (rapidjson::SizeType i = 0; i < entities.Size(); i++) {

		rapidjson::Value& e = entities[i];

		std::string file = e["filename"].GetString();
		
		std::string text;
		if (e.HasMember("texture")) {
			text = e["texture"].GetString();
		}

		chai3d::cVector3d position(0.0, 0.0, 0.0);
		if (e.HasMember("position")) {
			double x = e["position"]["x"].GetDouble();
			double y = e["position"]["y"].GetDouble();
			double z = e["position"]["z"].GetDouble();

			position = chai3d::cVector3d(x, y, z);
		}

		chai3d::cMatrix3d rotation;
		rotation.identity();
		if (e.HasMember("rotation")) {
			double x = e["rotation"]["x"].GetDouble();
			double y = e["rotation"]["y"].GetDouble();
			double z = e["rotation"]["z"].GetDouble();
			double deg = e["rotation"]["deg"].GetDouble();

			rotation = chai3d::cMatrix3d(x, y, z, deg * (M_PI / 180.0));
		}

		View view = (View) e["view"].GetInt();
		
		chai3d::cTransform trans(position, rotation);

		Entity newEntity = Entity(file, view, trans);
		if (e.HasMember("texture")) {
			newEntity.setTexture(text);
		}

		output.push_back(newEntity);

	}
}
