#pragma once

#include "models/bezierCurves/bezierCurveC0.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <unordered_map>
#include <vector>

class BezierCurveC0Serializer
{
public:
	static nlohmann::ordered_json serialize(const BezierCurveC0& curve, int id,
		const std::vector<std::unique_ptr<Point>>& points);
	static void deserialize(const nlohmann::json& json, Scene& scene,
		std::unordered_map<int, int> pointMap);
};
