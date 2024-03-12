#pragma once

#include <array>
#include <string>

constexpr int renderModeCount = 2;

enum class RenderMode
{
	wireframe,
	solid
};

inline std::array<std::string, renderModeCount> renderModeLabels = {"wireframe", "solid"};
