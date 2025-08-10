#include "serializer/models/torusSerializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json TorusSerializer::serialize(const Torus& torus, int& id)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "torus";
	json["name"] = torus.getName();
	json["id"] = id++;

	glm::vec3 pos = torus.getPos();
	json["position"]["x"] = pos.x;
	json["position"]["y"] = pos.y;
	json["position"]["z"] = pos.z;

	json["rotation"]["x"] = torus.getPitchRad();
	json["rotation"]["y"] = torus.getYawRad();
	json["rotation"]["z"] = torus.getRollRad();

	glm::vec3 scale = torus.getScale();
	json["scale"]["x"] = scale.x;
	json["scale"]["y"] = scale.y;
	json["scale"]["z"] = scale.z;

	json["samples"]["x"] = torus.m_minorGrid;
	json["samples"]["y"] = torus.m_majorGrid;

	json["smallRadius"] = torus.m_minorRadius;
	json["largeRadius"] = torus.m_majorRadius;

	return json;
}

void TorusSerializer::deserialize(const nlohmann::ordered_json& json, Scene& scene)
{
	scene.addTorus();
	Torus& torus = *scene.m_toruses.back();

	if (json.contains("name"))
	{
		torus.setName(json["name"]);
	}

	torus.setPos({json["position"]["x"], json["position"]["y"], json["position"]["z"]});

	torus.setPitchRad(json["rotation"]["x"]);
	torus.setYawRad(json["rotation"]["y"]);
	torus.setRollRad(json["rotation"]["z"]);

	torus.setScale({json["scale"]["x"], json["scale"]["y"], json["scale"]["z"]});

	torus.setMinorGrid(json["samples"]["x"]);
	torus.setMajorGrid(json["samples"]["y"]);

	torus.setMinorRadius(json["smallRadius"]);
	torus.setMajorRadius(json["largeRadius"]);
}
