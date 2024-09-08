#include "serializer/sceneSerializer.hpp"

#include "serializer/models/bezierCurves/bezierCurveC0Serializer.hpp"
#include "serializer/models/bezierCurves/bezierCurveC2Serializer.hpp"
#include "serializer/models/bezierCurves/bezierCurveInterSerializer.hpp"
#include "serializer/models/bezierSurfaces/bezierSurfaceC0Serializer.hpp"
#include "serializer/models/bezierSurfaces/bezierSurfaceC2Serializer.hpp"
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
	serializeBezierCurvesC0(geometryJson, scene.m_bezierCurvesC0, nonVirtualPoints, id);
	serializeBezierCurvesC2(geometryJson, scene.m_bezierCurvesC2, nonVirtualPoints, id);
	serializeBezierCurvesInter(geometryJson, scene.m_bezierCurvesInter, nonVirtualPoints, id);
	serializeBezierSurfacesC0(geometryJson, scene.m_bezierSurfacesC0, nonVirtualPoints, id);
	serializeBezierSurfacesC2(geometryJson, scene.m_bezierSurfacesC2, nonVirtualPoints, id);
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
			BezierCurveC0Serializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "bezierC2")
		{
			BezierCurveC2Serializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "interpolatedC2")
		{
			BezierCurveInterSerializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "bezierSurfaceC0")
		{
			BezierSurfaceC0Serializer::deserialize(modelJson, scene, pointMap);
		}
		else if (modelType == "bezierSurfaceC2")
		{
			BezierSurfaceC2Serializer::deserialize(modelJson, scene, pointMap);
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

void SceneSerializer::serializeBezierCurvesC0(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<BezierCurveC0>>& curves, const std::vector<Point*>& points,
	int& id)
{
	for (const std::unique_ptr<BezierCurveC0>& curve : curves)
	{
		nlohmann::ordered_json curveJson =
			BezierCurveC0Serializer::serialize(*curve, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeBezierCurvesC2(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<BezierCurveC2>>& curves, const std::vector<Point*>& points,
	int& id)
{
	for (const std::unique_ptr<BezierCurveC2>& curve : curves)
	{
		nlohmann::ordered_json curveJson =
			BezierCurveC2Serializer::serialize(*curve, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeBezierCurvesInter(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<BezierCurveInter>>& curves, const std::vector<Point*>& points,
	int& id)
{
	for (const std::unique_ptr<BezierCurveInter>& curve : curves)
	{
		nlohmann::ordered_json curveJson =
			BezierCurveInterSerializer::serialize(*curve, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeBezierSurfacesC0(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<BezierSurfaceC0>>& surfaces,
	const std::vector<Point*>& points, int& id)
{
	for (const std::unique_ptr<BezierSurfaceC0>& surface : surfaces)
	{
		nlohmann::ordered_json curveJson =
			BezierSurfaceC0Serializer::serialize(*surface, points, id);
		geometryJson.push_back(curveJson);
	}
}

void SceneSerializer::serializeBezierSurfacesC2(nlohmann::ordered_json& geometryJson,
	const std::vector<std::unique_ptr<BezierSurfaceC2>>& surfaces,
	const std::vector<Point*>& points, int& id)
{
	for (const std::unique_ptr<BezierSurfaceC2>& surface : surfaces)
	{
		nlohmann::ordered_json curveJson =
			BezierSurfaceC2Serializer::serialize(*surface, points, id);
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
	scene.m_bezierCurvesC0.clear();
	scene.m_bezierCurvesC2.clear();
	scene.m_bezierCurvesInter.clear();
	scene.m_bezierSurfacesC0.clear();
	scene.m_bezierSurfacesC2.clear();
	scene.m_gregorySurfaces.clear();
	scene.m_points.clear();
	scene.m_bezierCurvesToBeDeleted.clear();
}
