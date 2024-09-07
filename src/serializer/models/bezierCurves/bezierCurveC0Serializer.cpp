#include "serializer/models/bezierCurves/bezierCurveC0Serializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json BezierCurveC0Serializer::serialize(const BezierCurveC0& curve,
	const std::vector<Point*>& points, int& id)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "bezierC0";
	json["name"] = curve.getName();
	json["id"] = id++;

	for (const Point* curvePoint : curve.m_points)
	{
		nlohmann::ordered_json pointJson{};
		auto pointIterator = std::find_if
		(
			points.begin(), points.end(),
			[curvePoint] (const Point* point)
			{
				return point == curvePoint;
			}
		);
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
		points, scene.m_bezierCurveSelfDestructCallback);

	if (json.contains("name"))
	{
		curve->setName(json["name"]);
	}
	
	scene.m_models.push_back(curve.get());
	scene.m_bezierCurvesC0.push_back(std::move(curve));
}
