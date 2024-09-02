#pragma once

#include "models/bezierCurves/bezierCurveC2.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

class BezierCurveC2Serializer
{
public:
	static nlohmann::ordered_json serialize(const BezierCurveC2& curve, int id,
		const std::vector<std::unique_ptr<Point>>& points);
	static void deserialize(const nlohmann::ordered_json& json, Scene& scene,
		const std::unordered_map<int, int>& pointMap);
};
