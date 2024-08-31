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

inline const std::array<const std::string, bezierSurfaceWrappingCount> bezierSurfaceWrappingLabels
{
	"None",
	"U",
	"V"
};
