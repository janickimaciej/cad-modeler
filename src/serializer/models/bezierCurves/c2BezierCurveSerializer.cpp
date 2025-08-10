#include "serializer/models/bezierCurves/c2BezierCurveSerializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json C2BezierCurveSerializer::serialize(const C2BezierCurve& curve,
	const std::vector<Point*>& points, int& id)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "bezierC2";
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
		json["deBoorPoints"].push_back(pointJson);
	}

	return json;
}

void C2BezierCurveSerializer::deserialize(const nlohmann::ordered_json& json, Scene& scene,
	const std::unordered_map<int, int>& pointMap)
{
	std::vector<Point*> points{};
	for (const nlohmann::ordered_json& pointJson : json["deBoorPoints"])
	{
		int id = pointJson["id"];
		points.push_back(scene.m_points[pointMap.at(id)].get());
	}

	std::vector<std::unique_ptr<Point>> newPoints{};
	std::unique_ptr<C2BezierCurve> curve = std::make_unique<C2BezierCurve>(
		scene.m_shaderPrograms.bezierCurve, scene.m_shaderPrograms.polyline,
		scene.m_shaderPrograms.point, points, scene.m_bezierCurveSelfDestructCallback, newPoints);

	if (json.contains("name"))
	{
		curve->setName(json["name"]);
	}

	scene.addPoints(std::move(newPoints));
	scene.m_models.push_back(curve.get());
	scene.m_c2BezierCurves.push_back(std::move(curve));
}
