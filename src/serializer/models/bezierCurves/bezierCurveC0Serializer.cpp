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

void BezierCurveC0Serializer::deserialize(const nlohmann::ordered_json& json, Scene& scene,
	const std::unordered_map<int, int>& pointMap)
{
	std::vector<Point*> points{};
	for (const nlohmann::ordered_json& pointJson : json["controlPoints"])
	{
		int id = pointJson["id"];
		points.push_back(scene.m_points[pointMap.at(id)].get());
	}

	std::unique_ptr<BezierCurveC0> curve = std::make_unique<BezierCurveC0>(
		scene.m_shaderPrograms.bezierCurve, scene.m_shaderPrograms.polyline,
		points, scene.m_curveSelfDestructCallback);

	if (json.contains("name"))
	{
		curve->setName(json["name"]);
	}
	
	scene.m_models.push_back(curve.get());
	scene.m_bezierCurvesC0.push_back(std::move(curve));
}
