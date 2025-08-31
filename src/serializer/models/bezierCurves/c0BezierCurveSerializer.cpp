#include "serializer/models/bezierCurves/c0BezierCurveSerializer.hpp"

#include <glm/glm.hpp>

nlohmann::ordered_json C0BezierCurveSerializer::serialize(const C0BezierCurve& curve,
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

void C0BezierCurveSerializer::deserialize(const nlohmann::ordered_json& json, Scene& scene,
	const std::unordered_map<int, int>& pointMap)
{
	std::vector<Point*> points{};
	for (const nlohmann::ordered_json& pointJson : json["controlPoints"])
	{
		int id = pointJson["id"];
		points.push_back(scene.m_points[pointMap.at(id)].get());
	}

	std::unique_ptr<C0BezierCurve> curve = std::make_unique<C0BezierCurve>(
		points, scene.m_bezierCurveSelfDestructCallback);

	if (json.contains("name"))
	{
		curve->setName(json["name"]);
	}

	scene.m_models.push_back(curve.get());
	scene.m_c0BezierCurves.push_back(std::move(curve));
}
