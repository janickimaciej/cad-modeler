#include "serializer/sceneSerializer.hpp"

#include "serializer/models/bezierCurves/c0BezierCurveSerializer.hpp"
#include "serializer/models/bezierCurves/c2BezierCurveSerializer.hpp"
#include "serializer/models/bezierCurves/interpolatingBezierCurveSerializer.hpp"
#include "serializer/models/bezierSurfaces/c0BezierSurfaceSerializer.hpp"
#include "serializer/models/bezierSurfaces/c2BezierSurfaceSerializer.hpp"
#include "serializer/models/pointSerializer.hpp"
#include "serializer/models/torusSerializer.hpp"

#include <fstream>
#include <iomanip>
#include <unordered_map>

void SceneSerializer::serialize(Scene& scene, const std::string& path)
{
	nlohmann::ordered_json sceneJson{};
	int id = 0;

	std::vector<Point*> nonVirtualPoints = getNonVirtualPoints(scene);

	nlohmann::ordered_json pointsJson = nlohmann::ordered_json::array();
	serializePoints(pointsJson, nonVirtualPoints, id);
	sceneJson["points"] = pointsJson;

	nlohmann::ordered_json geometryJson = nlohmann::ordered_json::array();
	serializeToruses(geometryJson, scene.m_toruses, id);
	serializeC0BezierCurves(geometryJson, scene.m_c0BezierCurves, nonVirtualPoints, id);
	serializeC2BezierCurves(geometryJson, scene.m_c2BezierCurves, nonVirtualPoints, id);
	serializeInterpolatingBezierCurves(geometryJson, scene.m_interpolatingBezierCurves,
		nonVirtualPoints, id);
	serializeC0BezierSurfaces(geometryJson, scene.m_c0BezierSurfaces, nonVirtualPoints, id);
	serializeC2BezierSurfaces(geometryJson, scene.m_c2BezierSurfaces, nonVirtualPoints, id);
	sceneJson["geometry"] = geometryJson;

	std::ofstream file(path);
	if (file.fail())
	{
		return;
	}
	file << std::setw(4) << sceneJson << '\n';
}

void SceneSerializer::deserialize(Scene& scene, const std::string& path)
{
	clearScene(scene);

	std::ifstream file(path);
	if (file.fail())
	{
		return;
	}
	nlohmann::ordered_json sceneJson = nlohmann::ordered_json::parse(file);

	std::unordered_map<int, int> pointMap{};
	for (const nlohmann::ordered_json& pointJson : sceneJson["points"])
	{
		pointMap.insert(PointSerializer::deserialize(pointJson, scene));
	}

	for (const nlohmann::ordered_json& modelJson : sceneJson["geometry"])
	{
		std::string modelType = modelJson["objectType"];
		if (modelType == "torus")
		{
			TorusSerializer::deserialize(modelJson, scene);
		}
		else if (modelType == "bezierC0")
		{
			C0BezierCurveSerializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "bezierC2")
		{
			C2BezierCurveSerializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "interpolatedC2")
		{
			InterpolatingBezierCurveSerializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "bezierSurfaceC0")
		{
			C0BezierSurfaceSerializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "bezierSurfaceC2")
		{
			C2BezierSurfaceSerializer::deserialize(modelJson, scene, pointMap);
		}
	}
}

void SceneSerializer::serializePoints(nlohmann::ordered_json& pointsJson,
	const std::vector<Point*>& points, int& id)
{
	for (const Point* point : points)
	{
		nlohmann::ordered_json pointJson = PointSerializer::serialize(*point, id);
		pointsJson.push_back(pointJson);
	}
}

void SceneSerializer::serializeToruses(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<Torus>>& toruses, int& id)
{
	for (const std::unique_ptr<Torus>& torus : toruses)
	{
		nlohmann::ordered_json torusJson = TorusSerializer::serialize(*torus, id);
		geometryJson.push_back(torusJson);
	}
}

void SceneSerializer::serializeC0BezierCurves(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<C0BezierCurve>>& curves, const std::vector<Point*>& points,
	int& id)
{
	for (const std::unique_ptr<C0BezierCurve>& curve : curves)
	{
		nlohmann::ordered_json curveJson =
			C0BezierCurveSerializer::serialize(*curve, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeC2BezierCurves(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<C2BezierCurve>>& curves, const std::vector<Point*>& points,
	int& id)
{
	for (const std::unique_ptr<C2BezierCurve>& curve : curves)
	{
		nlohmann::ordered_json curveJson =
			C2BezierCurveSerializer::serialize(*curve, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeInterpolatingBezierCurves(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<InterpolatingBezierCurve>>& curves,
	const std::vector<Point*>& points, int& id)
{
	for (const std::unique_ptr<InterpolatingBezierCurve>& curve : curves)
	{
		nlohmann::ordered_json curveJson =
			InterpolatingBezierCurveSerializer::serialize(*curve, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeC0BezierSurfaces(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<C0BezierSurface>>& surfaces,
	const std::vector<Point*>& points, int& id)
{
	for (const std::unique_ptr<C0BezierSurface>& surface : surfaces)
	{
		nlohmann::ordered_json curveJson =
			C0BezierSurfaceSerializer::serialize(*surface, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeC2BezierSurfaces(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<C2BezierSurface>>& surfaces,
	const std::vector<Point*>& points, int& id)
{
	for (const std::unique_ptr<C2BezierSurface>& surface : surfaces)
	{
		nlohmann::ordered_json curveJson =
			C2BezierSurfaceSerializer::serialize(*surface, points, id);
		geometryJson.push_back(curveJson);
	}
}

std::vector<Point*> SceneSerializer::getNonVirtualPoints(const Scene& scene)
{
	std::vector<Point*> nonVirtualPoints{};
	for (const std::unique_ptr<Point>& point : scene.m_points)
	{
		if (!point->isVirtual())
		{
			nonVirtualPoints.push_back(point.get());
		}
	}
	return nonVirtualPoints;
}

void SceneSerializer::clearScene(Scene& scene)
{
	scene.deselectAllModels();
	scene.m_models.clear();
	scene.m_toruses.clear();
	scene.m_c0BezierCurves.clear();
	scene.m_c2BezierCurves.clear();
	scene.m_interpolatingBezierCurves.clear();
	scene.m_c0BezierSurfaces.clear();
	scene.m_c2BezierSurfaces.clear();
	scene.m_gregorySurfaces.clear();
	scene.m_intersectionCurves.clear();
	scene.m_points.clear();
	scene.m_bezierCurvesToBeDeleted.clear();
}
