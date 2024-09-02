#include "serializer/models/bezierCurves/bezierCurveC0Serializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json BezierCurveC0Serializer::serialize(const BezierCurveC0& bezierCurveC0,
	int id, const std::vector<std::unique_ptr<Point>>& points)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "bezierC0";
	json["name"] = bezierCurveC0.getName();
	json["id"] = id;

	for (const Point* curvePoint : bezierCurveC0.m_points)
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

void BezierCurveC0Serializer::deserialize(const nlohmann::json& json, Scene& scene,
	std::unordered_map<int, int> pointMap)
{
	for (const nlohmann::json& pointJson : json["controlPoints"])
	{
		int id = pointJson["id"];
		scene.selectModel(pointMap[id]);
	}
	scene.addBezierCurveC0();
	scene.deselectAllModels();
	BezierCurveC0& curve = *scene.m_bezierCurvesC0.back();

	if (json.contains("name"))
	{
		curve.setName(json["name"]);
	}
}
