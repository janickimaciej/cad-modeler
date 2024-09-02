#pragma once

#include "models/torus.hpp"
#include "scene.hpp"

#include <json/json.hpp>

class TorusSerializer
{
public:
	static nlohmann::ordered_json serialize(const Torus& torus, int id);
	static void deserialize(const nlohmann::json& json, Scene& scene);
};
