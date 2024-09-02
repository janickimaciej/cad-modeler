#pragma once

#include "models/bezierCurves/bezierCurveInter.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

class BezierCurveInterSerializer
{
public:
	static nlohmann::ordered_json serialize(const BezierCurveInter& curve, int id,
		const std::vector<std::unique_ptr<Point>>& points);
	static void deserialize(const nlohmann::ordered_json& json, Scene& scene,
		const std::unordered_map<int, int>& pointMap);
};
