#include "models/bezierSurfaces/bezierSurfaceC0.hpp"

#include <string>

BezierSurfaceC0::BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points) :
	BezierSurface{"BezierSurfaceC0 " + std::to_string(m_count++), bezierSurfaceShaderProgram,
	bezierSurfaceGridShaderProgram, patchesU, patchesV, wrapping}
{
	switch (wrapping)
	{
		case BezierSurfaceWrapping::none:
			m_pointsU = 3 * m_patchesU + 1;
			m_pointsV = 3 * m_patchesV + 1;
			break;

		case BezierSurfaceWrapping::u:
			m_pointsU = 3 * m_patchesU;
			m_pointsV = 3 * m_patchesV + 1;
			break;

		case BezierSurfaceWrapping::v:
			m_pointsU = 3 * m_patchesU + 1;
			m_pointsV = 3 * m_patchesV;
			break;
	}

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
	std::vector<std::vector<glm::vec3>> boorPoints = createBoorPoints(pos, sizeU, sizeV);
	std::vector<std::vector<glm::vec3>> bezierPoints = createBezierPoints(boorPoints);
	std::vector<std::unique_ptr<Point>> points{};
	m_points.resize(bezierPoints.size());
	for (int v = 0; v < bezierPoints.size(); ++v)
	{
		for (int u = 0; u < bezierPoints[v].size(); ++u)
		{
			points.push_back(std::make_unique<Point>(pointShaderProgram, bezierPoints[v][u],
				true));
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
	m_gridMesh = std::make_unique<Mesh>(createVertices(m_points), createGridIndices());
}

void BezierSurfaceC0::updateSurfaceMesh()
{
	m_surfaceMesh->update(createVertices(m_points), createSurfaceIndices());
}

void BezierSurfaceC0::updateGridMesh()
{
	m_gridMesh->update(createVertices(m_points), createGridIndices());
}
