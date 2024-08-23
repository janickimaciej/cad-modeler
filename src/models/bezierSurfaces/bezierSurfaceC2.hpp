#pragma once

#include "models/bezierSurfaces/bezierSurface.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class BezierSurfaceC2 : public BezierSurface
{
public:
	BezierSurfaceC2(const ShaderProgram& bezierSurfaceShaderProgram,
		const ShaderProgram& bezierSurfaceGridShaderProgram,
		const ShaderProgram& pointShaderProgram, int patchesU, int patchesV, const glm::vec3& pos,
		float sizeU, float sizeV, BezierSurfaceWrapping wrapping,
		std::vector<std::unique_ptr<Point>>& points);
	virtual ~BezierSurfaceC2() = default;

private:
	static int m_count;

	std::vector<std::vector<glm::vec3>> m_bezierPoints{};
	
	virtual std::vector<std::unique_ptr<Point>> createPoints(
		const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU,
		float sizeV) override;
	void createBezierPoints();
	virtual void createSurfaceMesh() override;
	virtual void createGridMesh() override;
	virtual void updateGeometry() override;
	void updateBezierPoints();
	std::vector<std::vector<glm::vec3>> createIntermediatePoints() const;
	virtual void updateSurfaceMesh() override;
	virtual void updateGridMesh() override;
	
	static std::vector<glm::vec3> createVertices(const std::vector<std::vector<glm::vec3>>& points);
};
