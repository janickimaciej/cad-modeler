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
	registerForNotifications();
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
	m_points.resize(pointsV);
	for (int v = 0; v < pointsV; ++v)
	{
		for (int u = 0; u < pointsU; ++u)
		{
			glm::vec3 pointPos = startCorner + glm::vec3{u * dU, 0, v * dV};
			points.push_back(std::make_unique<Point>(pointShaderProgram, pointPos, true));
			m_points[v].push_back(points.back().get());
		}
	}
	return points;
}

void BezierSurfaceC0::createSurfaceMesh()
{
	m_surfaceMesh = std::make_unique<BezierSurfaceMesh>(createVertices(m_points),
		createSurfaceIndices());
}

void BezierSurfaceC0::createGridMesh()
{
	m_gridMesh = std::make_unique<Mesh>(createVertices(m_points),
		createGridIndices(static_cast<int>(m_points[0].size()),
		static_cast<int>(m_points.size())));
}

void BezierSurfaceC0::updateSurfaceMesh()
{
	m_surfaceMesh->update(createVertices(m_points), createSurfaceIndices());
}

void BezierSurfaceC0::updateGridMesh()
{
	m_gridMesh->update(createVertices(m_points),
		createGridIndices(static_cast<int>(m_points[0].size()), static_cast<int>(m_points.size())));
}
