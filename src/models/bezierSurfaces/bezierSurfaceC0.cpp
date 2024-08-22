#include "models/bezierSurfaces/bezierSurfaceC0.hpp"

#include <string>

BezierSurfaceC0::BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points) :
	BezierSurface{"BezierSurfaceC0 " + std::to_string(m_count++), bezierSurfaceShaderProgram,
	bezierSurfaceGridShaderProgram, patchesU, patchesV}
{
	points = createPoints(pointShaderProgram, pos, sizeU, sizeV);
	updatePos();
	createSurfaceMesh();
	createGridMesh();
	registerForNotifications(m_points);
}

int BezierSurfaceC0::m_count = 0;

std::vector<std::unique_ptr<Point>> BezierSurfaceC0::createPoints(
	const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU, float sizeV)
{
	int pointsU = 3 * m_patchesU + 1;
	int pointsV = 3 * m_patchesV + 1;
	float dU = sizeU / (3 * m_patchesU);
	float dV = sizeV / (3 * m_patchesV);
	glm::vec3 startCorner = pos + glm::vec3{-sizeU / 2, 0, -sizeV / 2};
	std::vector<std::unique_ptr<Point>> points{};
	for (int v = 0; v < pointsV; ++v)
	{
		for (int u = 0; u < pointsU; ++u)
		{
			glm::vec3 pointPos = startCorner + glm::vec3{u * dU, 0, v * dV};
			points.push_back(std::make_unique<Point>(pointShaderProgram, pointPos, true));
			m_points.push_back(points.back().get());
		}
	}
	return points;
}

void BezierSurfaceC0::createSurfaceMesh()
{
	m_surfaceMesh = std::make_unique<BezierSurfaceMesh>(pointsToVertices(m_points),
		pointsToSurfaceIndices(m_points, m_patchesU, m_patchesV));
}

void BezierSurfaceC0::createGridMesh()
{
	m_gridMesh = std::make_unique<Mesh>(pointsToVertices(m_points),
		pointsToGridIndices(m_points, 3 * m_patchesU + 1, 3 * m_patchesV + 1));
}

void BezierSurfaceC0::updateSurfaceMesh()
{
	m_surfaceMesh->update(pointsToVertices(m_points), pointsToSurfaceIndices(m_points, m_patchesU,
		m_patchesV));
}

void BezierSurfaceC0::updateGridMesh()
{
	m_gridMesh->update(pointsToVertices(m_points), pointsToGridIndices(m_points, 3 * m_patchesU + 1,
		3 * m_patchesV + 1));
}
