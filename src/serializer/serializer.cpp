#include "serializer/serializer.hpp"

#include "serializer/models/bezierCurves/bezierCurveC0Serializer.hpp"
#include "serializer/models/bezierCurves/bezierCurveC2Serializer.hpp"
#include "serializer/models/bezierCurves/bezierCurveInterSerializer.hpp"
#include "serializer/models/pointSerializer.hpp"
#include "serializer/models/torusSerializer.hpp"

#include <fstream>
#include <iomanip>
#include <memory>
#include <unordered_map>

void Serializer::serialize(Scene& scene, const std::string& path)
{
	nlohmann::ordered_json sceneJson{};
	int id = 0;

	nlohmann::ordered_json pointsJson = nlohmann::ordered_json::array();
	serializePoints(pointsJson, scene, id);
	sceneJson["points"] = pointsJson;

	nlohmann::ordered_json geometryJson = nlohmann::ordered_json::array();
	serializeToruses(geometryJson, scene, id);
	serializeBezierCurvesC0(geometryJson, scene, id);
	serializeBezierCurvesC2(geometryJson, scene, id);
	serializeBezierCurvesInter(geometryJson, scene, id);
	sceneJson["geometry"] = geometryJson;

	std::ofstream file(path);
	if (file.fail())
	{
		return;
	}
	file << std::setw(4) << sceneJson << '\n';
}

void Serializer::deserialize(Scene& scene, const std::string& path)
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

		}
		else if (modelType == "bezierSurfaceC2")
		{

		}
	}
}

void Serializer::serializePoints(nlohmann::ordered_json& pointsJson, Scene& scene, int& id)
{
	for (const std::unique_ptr<Point>& point : scene.m_points)
	{
		if (point->isVirtual())
		{
			continue;
		}

		nlohmann::ordered_json pointJson = PointSerializer::serialize(*point, id++);
		pointsJson.push_back(pointJson);
	}
}

void Serializer::serializeToruses(nlohmann::ordered_json& geometryJson, Scene& scene, int& id)
{
	for (const std::unique_ptr<Torus>& torus : scene.m_toruses)
	{
		nlohmann::ordered_json torusJson = TorusSerializer::serialize(*torus, id++);
		geometryJson.push_back(torusJson);
	}
}

void Serializer::serializeBezierCurvesC0(nlohmann::ordered_json& geometryJson, Scene& scene,
	int& id)
{
	for (const std::unique_ptr<BezierCurveC0>& curve : scene.m_bezierCurvesC0)
	{
		nlohmann::ordered_json curveJson =
			BezierCurveC0Serializer::serialize(*curve, id++, scene.m_points);
		geometryJson.push_back(curveJson);
	}
}

void Serializer::serializeBezierCurvesC2(nlohmann::ordered_json& geometryJson, Scene& scene,
	int& id)
{
	for (const std::unique_ptr<BezierCurveC2>& curve : scene.m_bezierCurvesC2)
	{
		nlohmann::ordered_json curveJson =
			BezierCurveC2Serializer::serialize(*curve, id++, scene.m_points);
		geometryJson.push_back(curveJson);
	}
}

void Serializer::serializeBezierCurvesInter(nlohmann::ordered_json& geometryJson, Scene& scene,
	int& id)
{
	for (const std::unique_ptr<BezierCurveInter>& curve : scene.m_bezierCurvesInter)
	{
		nlohmann::ordered_json curveJson =
			BezierCurveInterSerializer::serialize(*curve, id++, scene.m_points);
		geometryJson.push_back(curveJson);
	}
}

void Serializer::clearScene(Scene& scene)
{
	scene.deselectAllModels();
	scene.m_models.clear();
	scene.m_points.clear();
	scene.m_toruses.clear();
	scene.m_bezierCurvesC0.clear();
	scene.m_bezierCurvesC2.clear();
	scene.m_bezierCurvesInter.clear();
	scene.m_bezierSurfacesC0.clear();
	scene.m_bezierSurfacesC2.clear();
	scene.m_bezierCurvesToBeDeleted.clear();
}
