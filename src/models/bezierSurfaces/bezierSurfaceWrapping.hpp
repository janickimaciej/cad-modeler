#pragma once

#include <array>
#include <string>

inline constexpr int bezierSurfaceWrappingCount = 3;

enum class BezierSurfaceWrapping
{
	none,
	u,
	v
};

inline std::array<std::string, bezierSurfaceWrappingCount> bezierSurfaceWrappingLabels{"none", "U",
	"V"};
