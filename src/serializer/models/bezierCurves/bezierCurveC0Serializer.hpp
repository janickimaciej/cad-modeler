#pragma once

#include "models/bezierCurves/bezierCurveC0.hpp"
#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

class BezierCurveC0Serializer
{
public:
	static nlohmann::ordered_json serialize(const BezierCurveC0& curve,
		const std::vector<Point*>& points, int& id);
	static void deserialize(const nlohmann::ordered_json& json, Scene& scene,
		const std::unordered_map<int, int>& pointMap);
};
