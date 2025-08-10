#pragma once

#include "models/bezierCurves/c0BezierCurve.hpp"
#include "models/bezierCurves/c2BezierCurve.hpp"
#include "models/bezierCurves/interpolatingBezierCurve.hpp"
#include "models/bezierSurfaces/c0BezierSurface.hpp"
#include "models/bezierSurfaces/c2BezierSurface.hpp"
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
	void serializeC0BezierCurves(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<C0BezierCurve>>& curves,
		const std::vector<Point*>& points, int& id);
	void serializeC2BezierCurves(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<C2BezierCurve>>& curves,
		const std::vector<Point*>& points, int& id);
	void serializeInterpolatingBezierCurves(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<InterpolatingBezierCurve>>& curves,
		const std::vector<Point*>& points, int& id);
	void serializeC0BezierSurfaces(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<C0BezierSurface>>& surfaces,
		const std::vector<Point*>& points, int& id);
	void serializeC2BezierSurfaces(nlohmann::ordered_json& geometryJson,
		const std::vector<std::unique_ptr<C2BezierSurface>>& surfaces,
		const std::vector<Point*>& points, int& id);

	std::vector<Point*> getNonVirtualPoints(const Scene& scene);
	void clearScene(Scene& scene);
};
