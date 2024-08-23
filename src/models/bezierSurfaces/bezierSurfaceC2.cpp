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
	std::size_t pointsU = static_cast<std::size_t>(m_patchesU) + 3;
	std::size_t pointsV = static_cast<std::size_t>(m_patchesV) + 3;
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

}

void BezierSurfaceC2::createSurfaceMesh()
{
	m_surfaceMesh = std::make_unique<BezierSurfaceMesh>(createVertices(m_points),
		createSurfaceIndices());
}

void BezierSurfaceC2::updateGeometry()
{
	updateBezierPoints();
	BezierSurface::updateGeometry();
}

void BezierSurfaceC2::updateBezierPoints()
{

}

void BezierSurfaceC2::createGridMesh()
{
	m_gridMesh = std::make_unique<Mesh>(createVertices(m_points),
		createGridIndices(m_patchesU + 3, m_patchesV + 3));
}

void BezierSurfaceC2::updateSurfaceMesh()
{
	m_surfaceMesh->update(createVertices(m_points), createSurfaceIndices());
}

void BezierSurfaceC2::updateGridMesh()
{
	m_gridMesh->update(createVertices(m_points), createGridIndices(m_patchesU + 3, m_patchesV + 3));
}
