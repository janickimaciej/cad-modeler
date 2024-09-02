#pragma once

#include "scene.hpp"

#include <json/json.hpp>

#include <string>

class Serializer
{
public:
	void serialize(Scene& scene, const std::string& path);
	void deserialize(Scene& scene, const std::string& path);

private:
	void serializePoints(nlohmann::ordered_json& pointsJson, Scene& scene, int& id);
	void serializeToruses(nlohmann::ordered_json& geometryJson, Scene& scene, int& id);
	void serializeBezierCurvesC0(nlohmann::ordered_json& geometryJson, Scene& scene, int& id);
	void serializeBezierCurvesC2(nlohmann::ordered_json& geometryJson, Scene& scene, int& id);
	void serializeBezierCurvesInter(nlohmann::ordered_json& geometryJson, Scene& scene, int& id);

	void clearScene(Scene& scene);
};
