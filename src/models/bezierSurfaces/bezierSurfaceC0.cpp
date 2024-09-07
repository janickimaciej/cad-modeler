#include "models/bezierSurfaces/bezierSurfaceC0.hpp"

#include <string>

BezierSurfaceC0::BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points) :
	BezierSurface{"C0 Bezier surface " + std::to_string(m_count++), bezierSurfaceShaderProgram,
	bezierSurfaceGridShaderProgram, patchesU, patchesV, wrapping}
{
	m_pointsU = getBezierPointsU();
	m_pointsV = getBezierPointsV();

	points = createPoints(pointShaderProgram, pos, sizeU, sizeV);
	updatePos();
	createSurfaceMesh();
	createGridMesh();
	registerForNotifications();
}

Point* BezierSurfaceC0::getCornerPointIfOnEdge(std::size_t patch, int corner)
{
	std::size_t patchU = patch % m_patchesU;
	std::size_t patchV = patch / m_patchesU;
	std::size_t startingCornerU = 3 * patchU;
	std::size_t startingCornerV = 3 * patchV;
	std::size_t endingCornerU = (startingCornerU + 3) % m_pointsU;
	std::size_t endingCornerV = (startingCornerV + 3) % m_pointsV;

	switch (corner)
	{
		case 0:
			if (startingCornerU == 0 || startingCornerV == 0)
			{
				return m_points[startingCornerV][startingCornerU];
			}
			break;

		case 1:
			if (endingCornerU == m_pointsU - 1 || startingCornerV == 0)
			{
				return m_points[startingCornerV][endingCornerU];
			}
			break;

		case 2:
			if (endingCornerU == m_pointsU - 1 || endingCornerV == m_pointsV - 1)
			{
				return m_points[endingCornerV][endingCornerU];
			}
			break;

		case 3:
			if (startingCornerU == 0 || endingCornerV == m_pointsV - 1)
			{
				return m_points[endingCornerV][startingCornerU];
			}
			break;
	}
	return nullptr;
}

std::array<std::array<Point*, 4>, 2> BezierSurfaceC0::getPointsBetweenCorners(std::size_t patch,
	int leftCorner, int rightCorner)
{
	return BezierSurface::getPointsBetweenCorners(m_points, patch, leftCorner, rightCorner);
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
				false));
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
