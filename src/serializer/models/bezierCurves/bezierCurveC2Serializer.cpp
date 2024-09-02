#include "serializer/models/bezierCurves/bezierCurveC2Serializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json BezierCurveC2Serializer::serialize(const BezierCurveC2& bezierCurveC2,
	int id, const std::vector<std::unique_ptr<Point>>& points)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "bezierC2";
	json["name"] = bezierCurveC2.getName();
	json["id"] = id;

	for (const Point* curvePoint : bezierCurveC2.m_points)
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
		json["deBoorPoints"].push_back(pointJson);
	}

	return json;
}

void BezierCurveC2Serializer::deserialize(const nlohmann::json& json, Scene& scene,
	std::unordered_map<int, int> pointMap)
{
	for (const nlohmann::json& pointJson : json["deBoorPoints"])
	{
		int id = pointJson["id"];
		scene.selectModel(pointMap[id]);
	}
	scene.addBezierCurveC2();
	scene.deselectAllModels();
	BezierCurveC2& curve = *scene.m_bezierCurvesC2.back();

	if (json.contains("name"))
	{
		curve.setName(json["name"]);
	}
}
