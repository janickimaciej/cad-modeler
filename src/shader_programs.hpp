#pragma once

#include "shader_program.hpp"

struct ShaderPrograms
{
	std::string shadersPath = "src/shaders/";
	ShaderProgram wireframe{shadersPath + "wireframe_vs.glsl",
		shadersPath + "wireframe_fs.glsl"};
	ShaderProgram solid{shadersPath + "solid_vs.glsl",
		shadersPath + "solid_fs.glsl"};
	ShaderProgram wireframePoint{shadersPath + "wireframe_point_vs.glsl",
		shadersPath + "wireframe_point_gs.glsl",
		shadersPath + "wireframe_point_fs.glsl"};
	ShaderProgram solidPoint{shadersPath + "solid_point_vs.glsl",
		shadersPath + "solid_point_gs.glsl",
		shadersPath + "solid_point_fs.glsl"};
	ShaderProgram cursor{shadersPath + "cursor_vs.glsl",
		shadersPath + "cursor_gs.glsl", shadersPath + "cursor_fs.glsl"};
	ShaderProgram grid{shadersPath + "grid_vs.glsl",
		shadersPath + "grid_fs.glsl"};
	ShaderProgram bezierCurvePolyline{shadersPath + "bezier_curve_polyline_vs.glsl",
		shadersPath + "bezier_curve_polyline_fs.glsl"};
	ShaderProgram bezierCurve{shadersPath + "bezier_curve_vs.glsl",
		shadersPath + "bezier_curve_tcs.glsl", shadersPath + "bezier_curve_tes.glsl",
		shadersPath + "bezier_curve_fs.glsl"};
};
