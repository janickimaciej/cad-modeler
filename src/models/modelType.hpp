#pragma once

#include <array>
#include <string>

inline constexpr int modelTypeCount = 9;

enum class ModelType
{
	all,
	point,
	torus,
	bezierCurveC0,
	bezierCurveC2,
	bezierCurveInter,
	bezierSurfaceC0,
	bezierSurfaceC2,
	gregorySurface
};

inline const std::array<std::string, modelTypeCount> modelTypeLabels
{
	"All",
	"Points",
	"Toruses",
	"C0 Bezier curves",
	"C2 Bezier curves",
	"Inter. Bezier curves",
	"C0 Bezier surfaces",
	"C2 Bezier surfaces",
	"Gregory surfaces"
};
