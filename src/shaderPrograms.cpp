#include "shaderPrograms.hpp"

#include <string>

namespace ShaderPrograms
{
	std::string path(const std::string& shaderName);

	std::unique_ptr<const ShaderProgram> point{};
	std::unique_ptr<const ShaderProgram> cursor{};
	std::unique_ptr<const ShaderProgram> plane{};
	std::unique_ptr<const ShaderProgram> torus{};
	std::unique_ptr<const ShaderProgram> bezierCurve{};
	std::unique_ptr<const ShaderProgram> heightmap{};
	std::unique_ptr<const ShaderProgram> edge{};
	std::unique_ptr<const ShaderProgram> interpolatingBezierCurve{};
	std::unique_ptr<const ShaderProgram> polyline{};
	std::unique_ptr<const ShaderProgram> bezierSurface{};
	std::unique_ptr<const ShaderProgram> bezierSurfaceTriangles{};
	std::unique_ptr<const ShaderProgram> gregorySurface{};
	std::unique_ptr<const ShaderProgram> quad{};
	std::unique_ptr<const ShaderProgram> vectors{};
	std::unique_ptr<const ShaderProgram> flat{};

	void init()
	{
		point = std::make_unique<const ShaderProgram>(path("pointVS"), path("pointGS"), path("FS"));
		cursor = std::make_unique<const ShaderProgram>(path("cursorVS"), path("cursorGS"),
			path("cursorFS"));
		plane = std::make_unique<const ShaderProgram>(path("planeVS"), path("planeFS"));
		torus = std::make_unique<const ShaderProgram>(path("torusVS"), path("intersectableFS"));
		bezierCurve = std::make_unique<const ShaderProgram>(path("bezierCurveVS"),
			path("bezierCurveTCS"), path("bezierCurveTES"), path("FS"));
		interpolatingBezierCurve = std::make_unique<const ShaderProgram>(
			path("interpolatingBezierCurveVS"), path("interpolatingBezierCurveTCS"),
			path("interpolatingBezierCurveTES"), path("FS"));
		heightmap = std::make_unique<const ShaderProgram>(path("heightmapVS"), path("heightmapFS"));
		edge = std::make_unique<const ShaderProgram>(path("edgeVS"), path("edgeFS"));
		polyline = std::make_unique<const ShaderProgram>(path("VS"), path("FS"));
		bezierSurface = std::make_unique<const ShaderProgram>(path("surfaceVS"),
			path("bezierSurfaceTCS"), path("bezierSurfaceTES"), path("intersectableFS"));
		bezierSurfaceTriangles = std::make_unique<const ShaderProgram>(path("surfaceVS"),
			path("bezierSurfaceTrianglesTCS"), path("bezierSurfaceTrianglesTES"),
			path("bezierSurfaceTrianglesFS"));
		gregorySurface = std::make_unique<const ShaderProgram>(path("surfaceVS"),
			path("gregorySurfaceTCS"), path("gregorySurfaceTES"), path("FS"));
		quad = std::make_unique<const ShaderProgram>(path("quadVS"), path("quadFS"));
		vectors = std::make_unique<const ShaderProgram>(path("VS"), path("vectorsFS"));
		flat = std::make_unique<const ShaderProgram>(path("flatVS"), path("FS"));
	}

	std::string path(const std::string& shaderName)
	{
		return "src/shaders/" + shaderName + ".glsl";
	}
}
