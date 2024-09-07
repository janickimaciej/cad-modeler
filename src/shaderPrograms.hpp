#pragma once

#include "shaderProgram.hpp"

#include <string>

struct ShaderPrograms
{
	ShaderProgram mesh{path("meshVS"), path("FS")};
	ShaderProgram point{path("pointVS"), path("pointGS"), path("FS")};
	ShaderProgram cursor{path("cursorVS"), path("cursorGS"), path("cursorFS")};
	ShaderProgram plane{path("planeVS"), path("planeFS")};
	ShaderProgram bezierCurve{path("bezierCurveVS"), path("bezierCurveTCS"), path("bezierCurveTES"),
		path("FS")};
	ShaderProgram bezierCurveInter{path("bezierCurveInterVS"), path("bezierCurveInterTCS"),
		path("bezierCurveInterTES"), path("FS")};
	ShaderProgram polyline{path("polylineVS"), path("FS")};
	ShaderProgram bezierSurface{path("bezierSurfaceVS"), path("bezierSurfaceTCS"),
		path("bezierSurfaceTES"), path("FS")};
	ShaderProgram gregorySurface{path("gregorySurfaceVS"), path("gregorySurfaceTCS"),
		path("gregorySurfaceTES"), path("FS")};
	ShaderProgram quad{path("quadVS"), path("quadFS")};

private:
	static std::string path(const std::string& shaderName);
};
