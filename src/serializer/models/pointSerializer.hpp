#pragma once

#include "models/point.hpp"
#include "scene.hpp"

#include <json/json.hpp>

#include <utility>

class PointSerializer
{
public:
	static nlohmann::ordered_json serialize(const Point& point, int id);
	static std::pair<int, int> deserialize(const nlohmann::json& json, Scene& scene);
};
