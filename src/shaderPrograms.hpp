#pragma once

#include "shaderProgram.hpp"

#include <string>

struct ShaderPrograms
{
	ShaderProgram mesh{path("meshVS"), path("FS")};
	ShaderProgram point{path("pointVS"), path("pointGS"), path("FS")};
	ShaderProgram cursor{path("cursorVS"), path("cursorGS"), path("cursorFS")};
	ShaderProgram plane{path("planeVS"), path("planeFS")};
	ShaderProgram torus{path("torusVS"), path("intersectableFS")};
	ShaderProgram bezierCurve{path("bezierCurveVS"), path("bezierCurveTCS"), path("bezierCurveTES"),
		path("FS")};
	ShaderProgram interpolatingBezierCurve{path("interpolatingBezierCurveVS"),
		path("interpolatingBezierCurveTCS"), path("interpolatingBezierCurveTES"), path("FS")};
	ShaderProgram polyline{path("VS"), path("FS")};
	ShaderProgram bezierSurface{path("surfaceVS"), path("bezierSurfaceTCS"),
		path("bezierSurfaceTES"), path("FS")};
	ShaderProgram gregorySurface{path("surfaceVS"), path("gregorySurfaceTCS"),
		path("gregorySurfaceTES"), path("FS")};
	ShaderProgram quad{path("quadVS"), path("quadFS")};
	ShaderProgram vectors{path("VS"), path("vectorsFS")};
	ShaderProgram flat{path("flatVS"), path("FS")};

private:
	static std::string path(const std::string& shaderName);
};
