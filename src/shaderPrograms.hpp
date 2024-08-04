#pragma once

#include "shaderProgram.hpp"

struct ShaderPrograms
{
	ShaderProgram torus{path("torusVS"), path("torusFS")};
	ShaderProgram point{path("pointVS"), path("pointGS"), path("pointFS")};
	ShaderProgram cursor{path("cursorVS"), path("cursorGS"), path("cursorFS")};
	ShaderProgram grid{path("gridVS"), path("gridFS")};
	ShaderProgram bezierCurvePolyline{path("bezierCurvePolylineVS"),
		path("bezierCurvePolylineFS")};
	ShaderProgram bezierCurve{path("bezierCurveVS"), path("bezierCurveTCS"),
		path("bezierCurveTES"), path("bezierCurveFS")};
	ShaderProgram bezierCurveInter{path("bezierCurveInterVS"), path("bezierCurveInterTCS"),
		path("bezierCurveInterTES"), path("bezierCurveInterFS")};

private:
	static std::string path(const std::string& shaderName);
};
