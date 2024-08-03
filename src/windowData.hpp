#pragma once

#include "guis/gui.hpp"
#include "scene.hpp"

#include <glm/glm.hpp>

struct WindowData
{
	Scene* scene{};
	GUI* gui{};

	glm::vec2 lastCursorPos{};
	bool dragging = false;
	bool rotatingRequested = false;
	bool scalingRequested = false;
};
