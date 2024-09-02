#pragma once

#include "models/bezierCurves/bezierCurveInter.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <unordered_map>
#include <vector>

class BezierCurveInterSerializer
{
public:
	static nlohmann::ordered_json serialize(const BezierCurveInter& curve, int id,
		const std::vector<std::unique_ptr<Point>>& points);
	static void deserialize(const nlohmann::json& json, Scene& scene,
		std::unordered_map<int, int> pointMap);
};
