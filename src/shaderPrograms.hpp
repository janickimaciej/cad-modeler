#pragma once

#include "shaderProgram.hpp"

#include <memory>

namespace ShaderPrograms
{
	void init();

	extern std::unique_ptr<const ShaderProgram> point;
	extern std::unique_ptr<const ShaderProgram> cursor;
	extern std::unique_ptr<const ShaderProgram> plane;
	extern std::unique_ptr<const ShaderProgram> torus;
	extern std::unique_ptr<const ShaderProgram> bezierCurve;
	extern std::unique_ptr<const ShaderProgram> interpolatingBezierCurve;
	extern std::unique_ptr<const ShaderProgram> polyline;
	extern std::unique_ptr<const ShaderProgram> bezierSurface;
	extern std::unique_ptr<const ShaderProgram> gregorySurface;
	extern std::unique_ptr<const ShaderProgram> quad;
	extern std::unique_ptr<const ShaderProgram> vectors;
	extern std::unique_ptr<const ShaderProgram> flat;
	extern std::unique_ptr<const ShaderProgram> bezierSurfaceTriangles;
	extern std::unique_ptr<const ShaderProgram> heightmap;
	extern std::unique_ptr<const ShaderProgram> edge;
}
