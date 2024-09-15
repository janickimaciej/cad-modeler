#pragma once

#include <array>
#include <string>

inline constexpr int modelTypeCount = 11;

enum class ModelType
{
	all,
	point,
	torus,
	bezierCurveC0,
	bezierCurveC2,
	bezierCurveInter,
	bezierPatch,
	bezierSurfaceC0,
	bezierSurfaceC2,
	gregorySurface,
	intersectionCurve
};

inline const std::array<std::string, modelTypeCount> modelTypeLabels
{
	"All",
	"Points",
	"Toruses",
	"C0 Bezier curves",
	"C2 Bezier curves",
	"Inter. Bezier curves",
	"Bezier patches",
	"C0 Bezier surfaces",
	"C2 Bezier surfaces",
	"Gregory surfaces",
	"Intersection curves"
};
