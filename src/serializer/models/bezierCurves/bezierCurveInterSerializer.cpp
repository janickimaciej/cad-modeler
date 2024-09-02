#include "serializer/models/bezierCurves/bezierCurveInterSerializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json BezierCurveInterSerializer::serialize(
	const BezierCurveInter& bezierCurveInter, int id,
	const std::vector<std::unique_ptr<Point>>& points)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "interpolatedC2";
	json["name"] = bezierCurveInter.getName();
	json["id"] = id;

	for (const Point* curvePoint : bezierCurveInter.m_points)
	{
		auto pointIterator = std::find_if
		(
			points.begin(), points.end(),
			[curvePoint] (const std::unique_ptr<Point>& point)
			{
				return point.get() == curvePoint;
			}
		);
		nlohmann::ordered_json pointJson{};
		pointJson["id"] = pointIterator - points.begin();
		json["controlPoints"].push_back(pointJson);
	}

	return json;
}

void BezierCurveInterSerializer::deserialize(const nlohmann::json& json, Scene& scene,
	std::unordered_map<int, int> pointMap)
{
	for (const nlohmann::json& pointJson : json["controlPoints"])
	{
		int id = pointJson["id"];
		scene.selectModel(pointMap[id]);
	}
	scene.addBezierCurveInter();
	scene.deselectAllModels();
	BezierCurveInter& curve = *scene.m_bezierCurvesInter.back();

	if (json.contains("name"))
	{
		curve.setName(json["name"]);
	}
}
