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
	updatePos();
	createSurfaceMesh();
	createGridMesh();
	registerForNotifications(m_points);
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
	for (std::size_t i = 0; i < pointsU * pointsV; ++i)
	{
		points.push_back(std::make_unique<Point>(pointShaderProgram, glm::vec3{}, true));
		m_points.push_back(points.back().get());
	}
	for (std::size_t v = 0; v < pointsV; ++v)
	{
		points[v * pointsU]->setPos(startCorner);
		points[v * pointsU + 1]->setPos(startCorner + glm::vec3{dU / 3.0f, 0, 0});
		for (std::size_t u = 2; u < pointsU - 2; ++u)
		{
			points[v * pointsU + u]->setPos(startCorner + glm::vec3{(u - 1) * dU, 0, 0});
		}
		points[v * pointsU + pointsU - 2]->setPos(startCorner +
			glm::vec3{(m_patchesU - 1.0 / 3.0f) * dU, 0, 0});
		points[v * pointsU + pointsU - 1]->setPos(startCorner + glm::vec3{m_patchesU * dU, 0, 0});
	}
	for (std::size_t u = 0; u < pointsU; ++u)
	{
		points[pointsU + u]->setPos(points[pointsU + u]->getPos() + glm::vec3{0, 0, dV / 3.0f});
		for (std::size_t v = 2; v < pointsV - 2; ++v)
		{
			points[v * pointsU + u]->setPos(points[v * pointsU + u]->getPos() +
				glm::vec3{0, 0, (v - 1) * dV});
		}
		points[(pointsV - 2) * pointsU + u]->setPos(points[(pointsV - 2) * pointsU + u]->getPos() +
			glm::vec3{0, 0, (m_patchesV - 1.0 / 3.0f) * dV});
		points[(pointsV - 1) * pointsU + u]->setPos(points[(pointsV - 1) * pointsU + u]->getPos() +
			glm::vec3{0, 0, m_patchesV * dV});
	}
	return points;
}

void BezierSurfaceC2::createSurfaceMesh()
{
	m_surfaceMesh = std::make_unique<BezierSurfaceMesh>(pointsToVertices(m_points),
		pointsToSurfaceIndices(m_points, m_patchesU, m_patchesV));
}

void BezierSurfaceC2::createGridMesh()
{
	m_gridMesh = std::make_unique<Mesh>(pointsToVertices(m_points),
		pointsToGridIndices(m_points, m_patchesU + 3, m_patchesV + 3));
}

void BezierSurfaceC2::updateSurfaceMesh()
{
	m_surfaceMesh->update(pointsToVertices(m_points), pointsToSurfaceIndices(m_points, m_patchesU,
		m_patchesV));
}

void BezierSurfaceC2::updateGridMesh()
{
	m_gridMesh->update(pointsToVertices(m_points), pointsToGridIndices(m_points, m_patchesU + 3,
		m_patchesV + 3));
}
