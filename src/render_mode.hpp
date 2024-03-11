#pragma once

#include <array>
#include <string>

constexpr int renderModeCount = 2;

enum class RenderMode
{
	solid,
	wireframe
};

inline std::array<std::string, renderModeCount> renderModeLabels = {"solid", "wireframe"};
