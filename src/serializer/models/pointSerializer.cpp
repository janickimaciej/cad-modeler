#include "serializer/models/pointSerializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json PointSerializer::serialize(const Point& point, int id)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "point";
	json["name"] = point.getName();
	json["id"] = id;

	json["position"]["x"] = point.m_pos.x;
	json["position"]["y"] = point.m_pos.y;
	json["position"]["z"] = point.m_pos.z;

	return json;
}

std::pair<int, int> PointSerializer::deserialize(const nlohmann::json& json, Scene& scene)
{
	scene.addPoint();
	Point& point = *scene.m_points.back();

	if (json.contains("name"))
	{
		point.setName(json["name"]);
	}

	point.setPos({json["position"]["x"], json["position"]["y"], json["position"]["z"]});

	return {json["id"], scene.m_points.size() - 1};
}
