#pragma once

#include "guis/gui.hpp"
#include "scene.hpp"

struct WindowUserData
{
	Scene* scene{};
	GUI* gui{};
};
