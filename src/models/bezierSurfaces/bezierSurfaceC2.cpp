#include "models/bezierSurfaces/bezierSurfaceC2.hpp"

#include <cstddef>
#include <string>

BezierSurfaceC2::BezierSurfaceC2(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& points) :
	BezierSurface{"BezierSurfaceC2 " + std::to_string(m_count++), bezierSurfaceShaderProgram,
		bezierSurfaceGridShaderProgram, patchesU, patchesV}
{
	points = createPoints(pointShaderProgram, pos, sizeU, sizeV);
	createBezierPoints();
	updateBezierPoints();
	updatePos();
	createSurfaceMesh();
	createGridMesh();
	registerForNotifications();
}

int BezierSurfaceC2::m_count = 0;

std::vector<std::unique_ptr<Point>> BezierSurfaceC2::createPoints(
	const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU, float sizeV)
{
	std::size_t pointsU = m_patchesU + 3;
	std::size_t pointsV = m_patchesV + 3;
	float dU = sizeU / m_patchesU;
	float dV = sizeV / m_patchesV;
	glm::vec3 startCorner = pos + glm::vec3{-sizeU / 2, 0, -sizeV / 2};
	std::vector<std::unique_ptr<Point>> points{};
	m_points.resize(pointsV);
	for (std::size_t v = 0; v < pointsV; ++v)
	{
		for (std::size_t u = 0; u < pointsU; ++u)
		{
			points.push_back(std::make_unique<Point>(pointShaderProgram, glm::vec3{}, true));
			m_points[v].push_back(points.back().get());
		}
	}
	for (std::size_t v = 0; v < pointsV; ++v)
	{
		m_points[v][0]->setPos(startCorner);
		m_points[v][1]->setPos(startCorner + glm::vec3{dU / 3.0f, 0, 0});
		for (std::size_t u = 2; u < pointsU - 2; ++u)
		{
			m_points[v][u]->setPos(startCorner + glm::vec3{(u - 1) * dU, 0, 0});
		}
		m_points[v][pointsU - 2]->setPos(startCorner +
			glm::vec3{(m_patchesU - 1.0 / 3.0f) * dU, 0, 0});
		m_points[v][pointsU - 1]->setPos(startCorner + glm::vec3{m_patchesU * dU, 0, 0});
	}
	for (std::size_t u = 0; u < pointsU; ++u)
	{
		m_points[1][u]->setPos(m_points[1][u]->getPos() + glm::vec3{0, 0, dV / 3.0f});
		for (std::size_t v = 2; v < pointsV - 2; ++v)
		{
			m_points[v][u]->setPos(m_points[v][u]->getPos() + glm::vec3{0, 0, (v - 1) * dV});
		}
		m_points[pointsV - 2][u]->setPos(m_points[pointsV - 2][u]->getPos() +
			glm::vec3{0, 0, (m_patchesV - 1.0 / 3.0f) * dV});
		m_points[pointsV - 1][u]->setPos(m_points[pointsV - 1][u]->getPos() +
			glm::vec3{0, 0, m_patchesV * dV});
	}
	return points;
}

void BezierSurfaceC2::createBezierPoints()
{
	m_bezierPoints.resize(3 * m_patchesV + 1);
	for (std::size_t v = 0; v < m_bezierPoints.size(); ++v)
	{
		m_bezierPoints[v].resize(3 * m_patchesU + 1);
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
		createGridIndices(static_cast<int>(m_patchesU) + 3, static_cast<int>(m_patchesV) + 3));
}

void BezierSurfaceC2::updateGeometry()
{
	updateBezierPoints();
	BezierSurface::updateGeometry();
}

void BezierSurfaceC2::updateBezierPoints()
{
	std::vector<std::vector<glm::vec3>> intermediatePoints = createIntermediatePoints();
	for (std::size_t u = 0; u < intermediatePoints[0].size(); ++u)
	{
		std::vector<glm::vec3> e(m_patchesV + 1);
		std::vector<glm::vec3> f(m_patchesV);
		std::vector<glm::vec3> g(m_patchesV);

		f[0] = intermediatePoints[1][u];
		g[0] = (intermediatePoints[1][u] + intermediatePoints[2][u]) / 2.0f;
		for (std::size_t patchV = 1; patchV < m_patchesV - 1; ++patchV)
		{
			f[patchV] = (2.0f * intermediatePoints[patchV + 1][u] +
				intermediatePoints[patchV + 2][u]) / 3.0f;
			g[patchV] = (intermediatePoints[patchV + 1][u] +
				2.0f * intermediatePoints[patchV + 2][u]) / 3.0f;
		}
		f[m_patchesV - 1] = (intermediatePoints[m_patchesV][u] +
			intermediatePoints[m_patchesV + 1][u]) / 2.0f;
		g[m_patchesV - 1] = intermediatePoints[m_patchesV + 1][u];

		e[0] = intermediatePoints[0][u];
		for (std::size_t patchV = 1; patchV < m_patchesV; ++patchV)
		{
			e[patchV] = (g[patchV - 1] + f[patchV]) / 2.0f;
		}
		e[m_patchesV] = intermediatePoints[m_patchesV + 2][u];

		for (std::size_t patchV = 0; patchV < m_patchesV; ++patchV)
		{
			m_bezierPoints[3 * patchV][u] = e[patchV];
			m_bezierPoints[3 * patchV + 1][u] = f[patchV];
			m_bezierPoints[3 * patchV + 2][u] = g[patchV];
		}
		m_bezierPoints[3 * m_patchesV][u] = e[m_patchesV];
	}
}

std::vector<std::vector<glm::vec3>> BezierSurfaceC2::createIntermediatePoints() const
{
	std::vector<std::vector<glm::vec3>> intermediatePoints(m_points.size());
	for (std::size_t v = 0; v < m_points.size(); ++v)
	{
		std::vector<glm::vec3> e(m_patchesU + 1);
		std::vector<glm::vec3> f(m_patchesU);
		std::vector<glm::vec3> g(m_patchesU);

		f[0] = m_points[v][1]->getPos();
		g[0] = (m_points[v][1]->getPos() + m_points[v][2]->getPos()) / 2.0f;
		for (std::size_t patchU = 1; patchU < m_patchesU - 1; ++patchU)
		{
			f[patchU] = (2.0f * m_points[v][patchU + 1]->getPos() +
				m_points[v][patchU + 2]->getPos()) / 3.0f;
			g[patchU] = (m_points[v][patchU + 1]->getPos() +
				2.0f * m_points[v][patchU + 2]->getPos()) / 3.0f;
		}
		f[m_patchesU - 1] = (m_points[v][m_patchesU]->getPos() +
			m_points[v][m_patchesU + 1]->getPos()) / 2.0f;
		g[m_patchesU - 1] = m_points[v][m_patchesU + 1]->getPos();

		e[0] = m_points[v][0]->getPos();
		for (std::size_t patchU = 1; patchU < m_patchesU; ++patchU)
		{
			e[patchU] = (g[patchU - 1] + f[patchU]) / 2.0f;
		}
		e[m_patchesU] = m_points[v][m_patchesU + 2]->getPos();

		for (std::size_t patchU = 0; patchU < m_patchesU; ++patchU)
		{
			intermediatePoints[v].push_back(e[patchU]);
			intermediatePoints[v].push_back(f[patchU]);
			intermediatePoints[v].push_back(g[patchU]);
		}
		intermediatePoints[v].push_back(e[m_patchesU]);
	}
	return intermediatePoints;
}

void BezierSurfaceC2::updateSurfaceMesh()
{
	m_surfaceMesh->update(createVertices(m_bezierPoints), createSurfaceIndices());
}

void BezierSurfaceC2::updateGridMesh()
{
	m_gridMesh->update(BezierSurface::createVertices(m_points),
		createGridIndices(static_cast<int>(m_patchesU) + 3, static_cast<int>(m_patchesV) + 3));
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
