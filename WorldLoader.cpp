#include "WorldLoader.h"

#include <string>

#include "Viscous.h"
#include "Hazard.h"
#include "Collectible.h"
#include "Magnet.h"

// Fills a vector of all entities from a world file and returns the time limit for the level
double WorldLoader::loadWorld(rapidjson::Document d, std::vector<Entity*>& output) {

	rapidjson::Value& entities = d["entities"];
	for (rapidjson::SizeType i = 0; i < entities.Size(); i++) {

		rapidjson::Value& e = entities[i];

		// OBJ file
		std::string file = e["filename"].GetString();
		
		// Texture file
		std::string text;
		if (e.HasMember("texture")) {
			text = e["texture"].GetString();
		}

		// World position
		chai3d::cVector3d position(0.0, 0.0, 0.0);
		if (e.HasMember("position")) {
			double x = e["position"]["x"].GetDouble();
			double y = e["position"]["y"].GetDouble();
			double z = e["position"]["z"].GetDouble();

			position = chai3d::cVector3d(x, y, z);
		}
		//position += chai3d::cVector3d(-0.55, 0.0, 0.0);

		// Rotations (angles about axis)
		chai3d::cMatrix3d rotation;
		rotation.identity();
		if (e.HasMember("rotation")) {
			double x = e["rotation"]["x"].GetDouble();
			double y = e["rotation"]["y"].GetDouble();
			double z = e["rotation"]["z"].GetDouble();
			double deg = e["rotation"]["deg"].GetDouble();

			rotation = chai3d::cMatrix3d(x, y, z, deg * (M_PI / 180.0));
		}
		chai3d::cTransform trans(position, rotation);

		// Player view
		View view = (View) e["view"].GetInt();

		std::string type = "entity";
		if (e.HasMember("type")) {
			type = e["type"].GetString();
		}
		
		// Create entity
		Entity* newEntity;
		if (type == "viscous") {
			newEntity = new Viscous(file, view, trans, e["damping"].GetDouble());
		}
		else if (type == "hazard") {
			newEntity = new Hazard(file, view, trans);
		}
		else if (type == "collectible") {
			newEntity = new Collectible(file, view, trans, e["bonus"].GetDouble());
		}
		else if (type == "magnet") {
			newEntity = new Magnet(file, view, trans, e["strength"].GetDouble());
		}
		else {
			newEntity = new Entity(file, view, trans);
		}
	
		// Set texture
		if (e.HasMember("texture")) {
			newEntity->setTexture(text);
		}
		output.push_back(newEntity);
	}
	return d["time"].GetDouble();
}
