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

class BezierSurfaceC0 : public BezierSurface
{
	friend class BezierSurfaceC0Serializer;

public:
	BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
		const ShaderProgram& bezierSurfaceGridShaderProgram,
		const ShaderProgram& pointShaderProgram, int patchesU, int patchesV, const glm::vec3& pos,
		float sizeU, float sizeV, BezierSurfaceWrapping wrapping,
		std::vector<std::unique_ptr<Point>>& points);
	virtual ~BezierSurfaceC0() = default;

	virtual Point* getCornerPointIfOnEdge(std::size_t patch, int corner) override;
	virtual std::array<std::array<Point*, 4>, 2> getPointsBetweenCorners(std::size_t patch,
		int leftCorner, int rightCorner) override;

private:
	static int m_count;
	
	virtual std::vector<std::unique_ptr<Point>> createPoints(
		const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU,
		float sizeV) override;
	virtual void createSurfaceMesh() override;
	virtual void createGridMesh() override;
	virtual void updateSurfaceMesh() override;
	virtual void updateGridMesh() override;
};
