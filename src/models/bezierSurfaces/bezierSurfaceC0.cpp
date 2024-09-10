#include "models/bezierSurfaces/bezierSurfaceC0.hpp"

#include <string>

BezierSurfaceC0::BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points,
	std::vector<std::unique_ptr<BezierPatch>>& patches) :
	BezierSurface{"C0 Bezier surface " + std::to_string(m_count++), bezierSurfaceGridShaderProgram,
		patchesU, patchesV, wrapping}
{
	m_pointsU = getBezierPointsU();
	m_pointsV = getBezierPointsV();

	points = createPoints(pointShaderProgram, pos, sizeU, sizeV);
	updatePos();
	patches = createPatches(bezierSurfaceShaderProgram);
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
				false));
			m_points[v].push_back(points.back().get());
		}
	}
	return points;
}

void BezierSurfaceC0::createGridMesh()
{
	m_gridMesh = std::make_unique<Mesh>(createVertices(m_points), createGridIndices());
}

void BezierSurfaceC0::updateGridMesh()
{
	m_gridMesh->update(createVertices(m_points), createGridIndices());
}

std::array<std::array<Point*, 4>, 4> BezierSurfaceC0::getBezierPoints(std::size_t patchU,
	std::size_t patchV) const
{
	std::array<std::array<Point*, 4>, 4> points{};
	std::size_t bezierPointsU = getBezierPointsU();
	std::size_t bezierPointsV = getBezierPointsV();
	for (int v = 0; v < 4; ++v)
	{
		for (int u = 0; u < 4; ++u)
		{
			points[v][u] = m_points[(3 * patchV + v) % bezierPointsV][(3 * patchU + u) %
				bezierPointsU];
		}
	}
	return points;
}
