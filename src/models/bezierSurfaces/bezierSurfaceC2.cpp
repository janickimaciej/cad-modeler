#include "models/bezierSurfaces/bezierSurfaceC2.hpp"

#include <cmath>
#include <cstddef>
#include <string>

BezierSurfaceC2::BezierSurfaceC2(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points) :
	BezierSurface{"C2 Bezier Surface " + std::to_string(m_count++), bezierSurfaceShaderProgram,
		bezierSurfaceGridShaderProgram, patchesU, patchesV, wrapping}
{
	switch (wrapping)
	{
		case BezierSurfaceWrapping::none:
			m_pointsU = m_patchesU + 3;
			m_pointsV = m_patchesV + 3;
			break;

		case BezierSurfaceWrapping::u:
			m_pointsU = m_patchesU;
			m_pointsV = m_patchesV + 3;
			break;

		case BezierSurfaceWrapping::v:
			m_pointsU = m_patchesU + 3;
			m_pointsV = m_patchesV;
			break;
	}

	points = createPoints(pointShaderProgram, pos, sizeU, sizeV);
	createBezierPoints();
	updatePos();
	createSurfaceMesh();
	createGridMesh();
	registerForNotifications();
}

int BezierSurfaceC2::m_count = 0;

std::vector<std::unique_ptr<Point>> BezierSurfaceC2::createPoints(
	const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU, float sizeV)
{
	std::vector<std::vector<glm::vec3>> boorPoints = createBoorPoints(pos, sizeU, sizeV);
	std::vector<std::unique_ptr<Point>> points{};
	m_points.resize(boorPoints.size());
	for (int v = 0; v < boorPoints.size(); ++v)
	{
		for (int u = 0; u < boorPoints[v].size(); ++u)
		{
			points.push_back(std::make_unique<Point>(pointShaderProgram, boorPoints[v][u], false));
			m_points[v].push_back(points.back().get());
		}
	}
	return points;
}

void BezierSurfaceC2::createBezierPoints()
{
	std::vector<std::vector<glm::vec3>> boorPoints(m_points.size());
	for (int v = 0; v < m_points.size(); ++v)
	{
		for (int u = 0; u < m_points[v].size(); ++u)
		{
			boorPoints[v].push_back(m_points[v][u]->getPos());
		}
	}
	m_bezierPoints = BezierSurface::createBezierPoints(boorPoints);
}

void BezierSurfaceC2::createSurfaceMesh()
{
	m_surfaceMesh = std::make_unique<BezierSurfaceMesh>(createVertices(m_bezierPoints),
		createSurfaceIndices());
}

void BezierSurfaceC2::createGridMesh()
{
	m_gridMesh = std::make_unique<Mesh>(BezierSurface::createVertices(m_points),
		createGridIndices());
}

void BezierSurfaceC2::updateGeometry()
{
	createBezierPoints();
	BezierSurface::updateGeometry();
}

void BezierSurfaceC2::updateSurfaceMesh()
{
	m_surfaceMesh->update(createVertices(m_bezierPoints), createSurfaceIndices());
}

void BezierSurfaceC2::updateGridMesh()
{
	m_gridMesh->update(BezierSurface::createVertices(m_points), createGridIndices());
}

std::vector<glm::vec3> BezierSurfaceC2::createVertices(
	const std::vector<std::vector<glm::vec3>>& points)
{
	std::vector<glm::vec3> vertices{};
	for (std::size_t v = 0; v < points.size(); ++v)
	{
		vertices.insert(vertices.end(), points[v].begin(), points[v].end());
	}
	return vertices;
}
