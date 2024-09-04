#pragma once

#include "models/bezierCurves/bezierCurveC0.hpp"
#include "models/bezierCurves/bezierCurveC2.hpp"
#include "models/bezierCurves/bezierCurveInter.hpp"
#include "models/bezierSurfaces/bezierSurfaceC0.hpp"
#include "models/bezierSurfaces/bezierSurfaceC2.hpp"
#include "models/point.hpp"
#include "models/torus.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <memory>
#include <string>
#include <vector>

class SceneSerializer
{
public:
	void serialize(Scene& scene, const std::string& path);
	void deserialize(Scene& scene, const std::string& path);

private:
	void serializePoints(nlohmann::ordered_json& pointsJson, const std::vector<Point*>& points,
		int& id);
	void serializeToruses(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<Torus>>& toruses, int& id);
	void serializeBezierCurvesC0(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<BezierCurveC0>>& curves,
		const std::vector<Point*>& points, int& id);
	void serializeBezierCurvesC2(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<BezierCurveC2>>& curves,
		const std::vector<Point*>& points, int& id);
	void serializeBezierCurvesInter(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<BezierCurveInter>>& curves,
		const std::vector<Point*>& points, int& id);
	void serializeBezierSurfacesC0(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<BezierSurfaceC0>>& surfaces,
		const std::vector<Point*>& points, int& id);
	void serializeBezierSurfacesC2(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<BezierSurfaceC2>>& surfaces,
		const std::vector<Point*>& points, int& id);

	std::vector<Point*> getNonVirtualPoints(const Scene& scene);
	void clearScene(Scene& scene);
};
