#include "models/gregorySurface.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <cstddef>
#include <string>

std::unique_ptr<GregorySurface> GregorySurface::create(const std::array<BezierPatch*, 3>& patches,
	const SelfDestructCallback& selfDestructCallback)
{
	std::optional<std::array<int, 6>> corners = find3Cycle(patches);
	if (!corners.has_value())
	{
		return nullptr;
	}

	return std::unique_ptr<GregorySurface>{new GregorySurface{patches, selfDestructCallback,
		*corners}};
}

void GregorySurface::render() const
{
	updateShaders();
	renderSurface();
	if (getRenderVectors())
	{
		renderVectors();
	}
}

void GregorySurface::updateGUI()
{
	m_gui.update();
}

void GregorySurface::setPos(const glm::vec3&)
{ }

bool GregorySurface::getRenderVectors() const
{
	return m_renderVectors;
}

void GregorySurface::setRenderVectors(bool renderVectors)
{
	m_renderVectors = renderVectors;
}

int GregorySurface::getLineCount() const
{
	return m_lineCount;
}

void GregorySurface::setLineCount(int lineCount)
{
	m_lineCount = lineCount;
}

int GregorySurface::m_count = 0;

GregorySurface::GregorySurface(const std::array<BezierPatch*, 3>& patches,
	const SelfDestructCallback& selfDestructCallback, const std::array<int, 6>& corners) :
	Model{{}, "Gregory surface " + std::to_string(m_count++)},
	m_selfDestructCallback{selfDestructCallback}
{
	getBezierPoints(patches, corners);
	createPoints();
	updatePos();
	createSurfaceMesh();
	createVectorsMesh();
	registerForNotifications(patches);
}

void GregorySurface::getBezierPoints(const std::array<BezierPatch*, 3>& patches,
	const std::array<int, 6>& corners)
{
	for (std::size_t i = 0; i < 3; ++i)
	{
		m_bezierPoints[i] = patches[i]->getPointsBetweenCorners(corners[2 * i], corners[2 * i + 1]);
	}
}

void GregorySurface::createPoints()
{
	std::array<std::array<std::array<glm::vec3, 3>, 2>, 3> R{};
	std::array<std::array<std::array<glm::vec3, 2>, 2>, 3> S{};
	std::array<std::array<glm::vec3, 2>, 3> T{};
	std::array<glm::vec3, 3> Q{};
	std::array<glm::vec3, 3> P3{};
	std::array<glm::vec3, 3> P2{};
	std::array<glm::vec3, 3> P1{};
	glm::vec3 P0{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		for (std::size_t row = 0; row < 2; ++row)
		{
			for (std::size_t point = 0; point < 3; ++point)
			{
				R[patch][row][point] = (m_bezierPoints[patch][row][point]->getPos() +
					m_bezierPoints[patch][row][point + 1]->getPos()) / 2.0f;
			}
			for (std::size_t point = 0; point < 2; ++point)
			{
				S[patch][row][point] = (R[patch][row][point] + R[patch][row][point + 1]) / 2.0f;
			}
			T[patch][row] = (S[patch][row][0] + S[patch][row][1]) / 2.0f;
		}

		P3[patch] = T[patch][0];
		P2[patch] = 2.0f * P3[patch] - T[patch][1];
		Q[patch] = (3.0f * P2[patch] - P3[patch]) / 2.0f;
	}
	P0 = (Q[0] + Q[1] + Q[2]) / 3.0f;
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		P1[patch] = (2.0f * Q[patch] + P0) / 3.0f;
	}

	std::array<glm::vec3, 3> g0{};
	std::array<glm::vec3, 3> g1{};
	std::array<glm::vec3, 3> g2{};
	std::array<glm::vec3, 3> g{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		g0[patch] = S[patch][0][1] - T[patch][0];
		glm::vec3 a = P0 - P1[(patch + 2) % 3];
		glm::vec3 b = P1[(patch + 1) % 3] - P0;
		g2[patch] = (a + b) / 2.0f;
		g1[patch] = (g0[patch] + g2[patch]) / 2.0f;
		g[patch] = deCasteljau(g0[patch], g1[patch], g2[patch], 2.0f / 3.0f);
	}

	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		m_points[patch][0] = P3[patch];
		m_points[patch][1] = P2[patch];
		m_points[patch][2] = P1[patch];
		m_points[patch][3] = P0;

		m_points[patch][4] = S[patch][0][1];
		m_points[patch][5] = m_points[patch][6] = 2.0f * S[patch][0][1] - S[patch][1][1];
		m_points[patch][7] = m_points[patch][2] + g[patch];
		m_points[patch][8] = P1[(patch + 1) % 3] - g[(patch + 1) % 3];
		m_points[patch][9] = P1[(patch + 1) % 3];

		m_points[patch][10] = R[patch][0][2];
		m_points[patch][11] = 2.0f * R[patch][0][2] - R[patch][1][2];
		m_points[patch][12] = 2.0f * R[(patch + 1) % 3][0][0] - R[(patch + 1) % 3][1][0];
		m_points[patch][13] = m_points[patch][14] =
			2.0f * S[(patch + 1) % 3][0][0] - S[(patch + 1) % 3][1][0];
		m_points[patch][15] = P2[(patch + 1) % 3];

		m_points[patch][16] = m_bezierPoints[patch][0][3]->getPos();
		m_points[patch][17] = R[(patch + 1) % 3][0][0];
		m_points[patch][18] = S[(patch + 1) % 3][0][0];
		m_points[patch][19] = P3[(patch + 1) % 3];
	}
}

std::vector<glm::vec3> GregorySurface::createVectors() const
{
	std::vector<glm::vec3> vectors{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		vectors.push_back(m_points[patch][1]);
		vectors.push_back(m_points[patch][6]);

		vectors.push_back(m_points[patch][2]);
		vectors.push_back(m_points[patch][7]);

		vectors.push_back(m_points[patch][9]);
		vectors.push_back(m_points[patch][8]);

		vectors.push_back(m_points[patch][15]);
		vectors.push_back(m_points[patch][14]);

		vectors.push_back(m_points[patch][18]);
		vectors.push_back(m_points[patch][13]);

		vectors.push_back(m_points[patch][17]);
		vectors.push_back(m_points[patch][12]);

		vectors.push_back(m_points[patch][10]);
		vectors.push_back(m_points[patch][11]);

		vectors.push_back(m_points[patch][4]);
		vectors.push_back(m_points[patch][5]);
	}
	return vectors;
}

void GregorySurface::createSurfaceMesh()
{
	std::vector<glm::vec3> points{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		points.insert(points.end(), m_points[patch].begin(), m_points[patch].end());
	}
	m_surfaceMesh = std::make_unique<Mesh>(points, GL_PATCHES, 20);
}

void GregorySurface::createVectorsMesh()
{
	m_vectorsMesh = std::make_unique<Mesh>(createVectors(), GL_LINES);
}

void GregorySurface::updateGeometry()
{
	updatePos();
	createPoints();
	updateSurfaceMesh();
	updateVectorsMesh();
}

void GregorySurface::updatePos()
{
	glm::vec3 posSum{};
	for (const std::array<glm::vec3, 20>& patch : m_points)
	{
		for (const glm::vec3& point : patch)
		{
			posSum += point;
		}
	}
	Model::setPos(posSum / static_cast<float>(m_points.size() * m_points[0].size()));
}

void GregorySurface::updateSurfaceMesh()
{
	std::vector<glm::vec3> points{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		points.insert(points.end(), m_points[patch].begin(), m_points[patch].end());
	}
	m_surfaceMesh->update(points);
}

void GregorySurface::updateVectorsMesh()
{
	m_vectorsMesh->update(createVectors());
}

void GregorySurface::updateShaders() const
{
	m_surfaceShaderProgram.use();
	m_surfaceShaderProgram.setUniform("lineCount", m_lineCount);
	m_surfaceShaderProgram.setUniform("isDark", false);
	m_surfaceShaderProgram.setUniform("isSelected", isSelected());
}

void GregorySurface::renderSurface() const
{
	m_surfaceShaderProgram.use();
	m_surfaceShaderProgram.setUniform("orientationFlipped", false);
	m_surfaceMesh->render();
	m_surfaceShaderProgram.setUniform("orientationFlipped", true);
	m_surfaceMesh->render();
}

void GregorySurface::renderVectors() const
{
	m_vectorsShaderProgram.use();
	m_vectorsMesh->render();
}

void GregorySurface::registerForNotifications(const std::array<BezierPatch*, 3>& patches)
{
	for (const std::array<std::array<Point*, 4>, 2>& patchPoints : m_bezierPoints)
	{
		for (const std::array<Point*, 4>& row : patchPoints)
		{
			for (Point* point : row)
			{
				registerForNotifications(point);
			}
		}
	}

	for (BezierPatch* patch : patches)
	{
		m_patchDestroyNotifications.push_back(patch->registerForDestroyNotification
			(
				[this] ()
				{
					surfaceDestroyNotification();
				}
			));
	}
}

void GregorySurface::registerForNotifications(Point* point)
{
	m_pointMoveNotifications.push_back(point->registerForMoveNotification
		(
			[this] (Point*)
			{
				pointMoveNotification();
			}
		));

	m_pointRereferenceNotifications.push_back(point->registerForRereferenceNotification
		(
			[this] (Point* point, Point* newPoint)
			{
				pointRereferenceNotification(point, newPoint);
			}
		));

	m_pointDeletabilityLocks.push_back(point->acquireDeletabilityLock());
}

void GregorySurface::pointMoveNotification()
{
	updateGeometry();
}

void GregorySurface::pointRereferenceNotification(const Point* point, Point* newPoint)
{
	int patchIndex{};
	int rowIndex{};
	int columnIndex{};
	std::tie(patchIndex, rowIndex, columnIndex) = getBezierPointIndices(point);

	m_bezierPoints[patchIndex][rowIndex][columnIndex] = newPoint;

	updateGeometry();
}

void GregorySurface::surfaceDestroyNotification()
{
	m_pointMoveNotifications.clear();
	m_pointRereferenceNotifications.clear();
	m_patchDestroyNotifications.clear();
	m_selfDestructCallback(this);
}

glm::vec3 GregorySurface::deCasteljau(const glm::vec3& a, const glm::vec3& b, float t)
{
	return (1 - t) * a + t * b;
}

glm::vec3 GregorySurface::deCasteljau(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	float t)
{
	return (1 - t) * deCasteljau(a, b, t) + t * deCasteljau(b, c, t);
}

std::optional<std::array<int, 6>> GregorySurface::find3Cycle(
	const std::array<BezierPatch*, 3>& patches)
{
	for (int i1 = 0; i1 < 4; ++i1)
	{
		Point* i1Point = patches[0]->getCornerPointIfOnEdge(i1);
		if (i1Point != nullptr)
		{
			std::array<int, 2> i2 = {(i1 + 1) % 4, (i1 + 3) % 4};
			for (int i2Index = 0; i2Index < 2; ++i2Index)
			{
				for (int j1 = 0; j1 < 4; ++j1)
				{
					Point* i2Point = patches[0]->getCornerPointIfOnEdge(i2[i2Index]);
					Point* j1Point = patches[1]->getCornerPointIfOnEdge(j1);
					if (i2Point != nullptr && j1Point == i2Point)
					{
						std::array<int, 2> j2 = {(j1 + 1) % 4, (j1 + 3) % 4};
						for (int j2Index = 0; j2Index < 2; ++j2Index)
						{
							for (int k1 = 0; k1 < 4; ++k1)
							{
								Point* j2Point = patches[1]->getCornerPointIfOnEdge(j2[j2Index]);
								Point* k1Point = patches[2]->getCornerPointIfOnEdge(k1);
								if (j2Point != nullptr && k1Point == j2Point)
								{
									std::array<int, 2> k2 = {(k1 + 1) % 4, (k1 + 3) % 4};
									for (int k2Index = 0; k2Index < 2; ++k2Index)
									{
										Point* k2Point = patches[2]->getCornerPointIfOnEdge(
											k2[k2Index]);
										if (i1Point == k2Point)
										{
											return std::array<int, 6>
											{
												i1, i2[i2Index],
												j1, j2[j2Index],
												k1, k2[k2Index]
											};
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return std::nullopt;
}

std::tuple<int, int, int> GregorySurface::getBezierPointIndices(const Point* point) const
{
	for (int patch = 0; patch < m_bezierPoints.size(); ++patch)
	{
		for (int row = 0; row < m_bezierPoints[patch].size(); ++row)
		{
			for (int column = 0; column < m_bezierPoints[patch][row].size(); ++column)
			{
				if (m_bezierPoints[patch][row][column] == point)
				{
					return {patch, row, column};
				}
			}
		}
	}
	return {};
}
