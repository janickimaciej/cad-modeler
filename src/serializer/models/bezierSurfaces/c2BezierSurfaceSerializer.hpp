#pragma once

#include "models/bezierSurfaces/c2BezierSurface.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <unordered_map>
#include <vector>

class C2BezierSurfaceSerializer
{
public:
	static nlohmann::ordered_json serialize(const C2BezierSurface& surface,
		const std::vector<Point*>& points, int& id);
	static void deserialize(const nlohmann::ordered_json& json, Scene& scene,
		const std::unordered_map<int, int>& pointMap);
};
