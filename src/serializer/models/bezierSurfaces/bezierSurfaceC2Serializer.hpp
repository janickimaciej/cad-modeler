#pragma once

#include "models/bezierSurfaces/bezierSurfaceC2.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <unordered_map>
#include <vector>

class BezierSurfaceC2Serializer
{
public:
	static nlohmann::ordered_json serialize(const BezierSurfaceC2& surface,
		const std::vector<Point*>& points, int& id);
	static void deserialize(const nlohmann::ordered_json& json, Scene& scene,
		const std::unordered_map<int, int>& pointMap);
};
