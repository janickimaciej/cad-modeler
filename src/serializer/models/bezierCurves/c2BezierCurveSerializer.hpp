#pragma once

#include "models/bezierCurves/c2BezierCurve.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

class C2BezierCurveSerializer
{
public:
	static nlohmann::ordered_json serialize(const C2BezierCurve& curve,
		const std::vector<Point*>& points, int& id);
	static void deserialize(const nlohmann::ordered_json& json, Scene& scene,
		const std::unordered_map<int, int>& pointMap);
};
