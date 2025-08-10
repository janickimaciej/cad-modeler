#pragma once

#include <array>
#include <string>

inline constexpr int modelTypeCount = 11;

enum class ModelType
{
	all,
	point,
	torus,
	c0BezierCurve,
	c2BezierCurve,
	interpolatingBezierCurve,
	bezierPatch,
	c0BezierSurface,
	c2BezierSurface,
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
	"Interpolating curves",
	"Bezier patches",
	"C0 Bezier surfaces",
	"C2 Bezier surfaces",
	"Gregory surfaces",
	"Intersection curves"
};
