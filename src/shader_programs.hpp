#pragma once

#include "shader_program.hpp"

struct ShaderPrograms
{
	ShaderProgram wireframe{path("wireframe_vs"), path("wireframe_fs")};
	ShaderProgram solid{path("solid_vs"), path("solid_fs")};
	ShaderProgram point{path("point_vs"), path("point_gs"), path("point_fs")};
	ShaderProgram cursor{path("cursor_vs"), path("cursor_gs"), path("cursor_fs")};
	ShaderProgram grid{path("grid_vs"), path("grid_fs")};
	ShaderProgram bezierCurvePolyline{path("bezier_curve_polyline_vs"),
		path("bezier_curve_polyline_fs")};
	ShaderProgram bezierCurve{path("bezier_curve_vs"), path("bezier_curve_tcs"),
		path("bezier_curve_tes"), path("bezier_curve_fs")};
	ShaderProgram bezierCurveInter{path("bezier_curve_inter_vs"), path("bezier_curve_inter_tcs"),
		path("bezier_curve_inter_tes"), path("bezier_curve_inter_fs")};

private:
	static std::string path(const std::string& shaderName);
};
