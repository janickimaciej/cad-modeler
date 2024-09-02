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

void BezierCurveC2Serializer::deserialize(const nlohmann::ordered_json& json, Scene& scene,
	const std::unordered_map<int, int>& pointMap)
{
	std::vector<Point*> points{};
	for (const nlohmann::ordered_json& pointJson : json["deBoorPoints"])
	{
		int id = pointJson["id"];
		points.push_back(scene.m_points[pointMap.at(id)].get());
	}
	
	std::vector<std::unique_ptr<Point>> newPoints{};
	std::unique_ptr<BezierCurveC2> curve = std::make_unique<BezierCurveC2>(
		scene.m_shaderPrograms.bezierCurve, scene.m_shaderPrograms.polyline,
		scene.m_shaderPrograms.point, points, scene.m_curveSelfDestructCallback, newPoints);

	if (json.contains("name"))
	{
		curve->setName(json["name"]);
	}
	
	scene.addPoints(std::move(newPoints));
	scene.m_models.push_back(curve.get());
	scene.m_bezierCurvesC2.push_back(std::move(curve));
}
