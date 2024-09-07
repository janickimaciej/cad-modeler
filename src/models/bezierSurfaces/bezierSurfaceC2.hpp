#pragma once

#include "models/bezierSurfaces/bezierSurface.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

class BezierSurfaceC2 : public BezierSurface
{
	friend class BezierSurfaceC2Serializer;

public:
	BezierSurfaceC2(const ShaderProgram& bezierSurfaceShaderProgram,
		const ShaderProgram& bezierSurfaceGridShaderProgram,
		const ShaderProgram& pointShaderProgram, int patchesU, int patchesV, const glm::vec3& pos,
		float sizeU, float sizeV, BezierSurfaceWrapping wrapping,
		std::vector<std::unique_ptr<Point>>& points);
	virtual ~BezierSurfaceC2() = default;

	virtual Point* getCornerPointIfOnEdge(std::size_t patch, int corner) override;
	virtual std::array<std::array<Point*, 4>, 2> getPointsBetweenCorners(std::size_t patch,
		int leftCorner, int rightCorner) override;

private:
	static int m_count;

	std::vector<std::vector<std::unique_ptr<Point>>> m_bezierPoints{};
	
	virtual std::vector<std::unique_ptr<Point>> createPoints(
		const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU,
		float sizeV) override;
	void createBezierPoints(const ShaderProgram& pointShaderProgram);
	void updateBezierPoints();
	virtual void createSurfaceMesh() override;
	virtual void createGridMesh() override;
	virtual void updateGeometry() override;
	virtual void updateSurfaceMesh() override;
	virtual void updateGridMesh() override;
};
