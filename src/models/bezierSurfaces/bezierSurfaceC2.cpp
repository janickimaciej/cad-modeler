#include "models/bezierSurfaces/bezierSurfaceC2.hpp"

#include <cmath>
#include <cstddef>
#include <string>

BezierSurfaceC2::BezierSurfaceC2(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points) :
	BezierSurface{"C2 Bezier surface " + std::to_string(m_count++), bezierSurfaceShaderProgram,
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
	createBezierPoints(pointShaderProgram);
	updateBezierPoints();
	updatePos();
	createSurfaceMesh();
	createGridMesh();
	registerForNotifications();
}

Point* BezierSurfaceC2::getCornerPointIfOnEdge(std::size_t patch, int corner)
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
				return m_bezierPoints[startingCornerV][startingCornerU].get();
			}
			break;

		case 1:
			if (endingCornerU == m_pointsU - 1 || startingCornerV == 0)
			{
				return m_bezierPoints[startingCornerV][endingCornerU].get();
			}
			break;

		case 2:
			if (endingCornerU == m_pointsU - 1 || endingCornerV == m_pointsV - 1)
			{
				return m_bezierPoints[endingCornerV][endingCornerU].get();
			}
			break;

		case 3:
			if (startingCornerU == 0 || endingCornerV == m_pointsV - 1)
			{
				return m_bezierPoints[endingCornerV][startingCornerU].get();
			}
			break;
	}
	return {};
}

std::array<std::array<Point*, 4>, 2> BezierSurfaceC2::getPointsBetweenCorners(std::size_t patch,
	int leftCorner, int rightCorner)
{
	return BezierSurface::getPointsBetweenCorners(m_bezierPoints, patch, leftCorner, rightCorner);
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

void BezierSurfaceC2::createBezierPoints(const ShaderProgram& pointShaderProgram)
{
	m_bezierPoints.resize(getBezierPointsV());
	for (std::vector<std::unique_ptr<Point>>& row : m_bezierPoints)
	{
		for (int u = 0; u < getBezierPointsU(); ++u)
		{
			row.push_back(std::make_unique<Point>(pointShaderProgram, glm::vec3{}));
		}
	}
}

void BezierSurfaceC2::updateBezierPoints()
{
	std::vector<std::vector<glm::vec3>> boorPoints(m_points.size());
	for (int v = 0; v < m_points.size(); ++v)
	{
		for (int u = 0; u < m_points[v].size(); ++u)
		{
			boorPoints[v].push_back(m_points[v][u]->getPos());
		}
	}

	std::vector<std::vector<glm::vec3>> bezierPoints =
		BezierSurface::createBezierPoints(boorPoints);
	for (int v = 0; v < bezierPoints.size(); ++v)
	{
		for (int u = 0; u < bezierPoints[0].size(); ++u)
		{
			m_bezierPoints[v][u]->setPos(bezierPoints[v][u]);
		}
	}
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
	updateBezierPoints();
	BezierSurface::updateGeometry();
}

void BezierSurfaceC2::updateSurfaceMesh()
{
	m_surfaceMesh->update(createVertices(m_bezierPoints), createSurfaceIndices());
}

void BezierSurfaceC2::updateGridMesh()
{
	m_gridMesh->update(createVertices(m_points), createGridIndices());
}
