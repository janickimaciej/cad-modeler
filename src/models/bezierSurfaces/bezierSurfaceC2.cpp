#include "models/bezierSurfaces/bezierSurfaceC2.hpp"

#include "meshes/indicesMesh.hpp"

#include <glad/glad.h>

#include <cmath>
#include <cstddef>
#include <string>

BezierSurfaceC2::BezierSurfaceC2(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points,
	std::vector<std::unique_ptr<BezierPatch>>& patches) :
	BezierSurface{"C2 Bezier surface " + std::to_string(m_count++), bezierSurfaceGridShaderProgram,
		patchesU, patchesV, wrapping}
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
	patches = createPatches(bezierSurfaceShaderProgram);
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

void BezierSurfaceC2::createGridMesh()
{
	m_gridMesh = std::make_unique<IndicesMesh>(BezierSurface::createVertices(m_points),
		createGridIndices(), GL_LINES);
}

void BezierSurfaceC2::updateGeometry()
{
	updateBezierPoints();
	BezierSurface::updateGeometry();
}

void BezierSurfaceC2::updateGridMesh()
{
	m_gridMesh->update(createVertices(m_points));
	m_gridMesh->update(createGridIndices());
}

std::array<std::array<Point*, 4>, 4> BezierSurfaceC2::getBezierPoints(std::size_t patchU,
	std::size_t patchV) const
{
	std::array<std::array<Point*, 4>, 4> points{};
	std::size_t bezierPointsU = getBezierPointsU();
	std::size_t bezierPointsV = getBezierPointsV();
	for (int v = 0; v < 4; ++v)
	{
		for (int u = 0; u < 4; ++u)
		{
			points[v][u] = m_bezierPoints[(3 * patchV + v) % bezierPointsV][(3 * patchU + u) %
				bezierPointsU].get();
		}
	}
	return points;
}
