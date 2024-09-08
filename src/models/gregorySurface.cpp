#include "models/gregorySurface.hpp"

#include <algorithm>
#include <string>

GregorySurface::GregorySurface(const ShaderProgram& surfaceShaderProgram,
	const ShaderProgram& vectorsShaderProgram,
	const std::array<std::array<std::array<Point*, 4>, 2>, 3>& bezierPoints,
	const SelfDestructCallback& selfDestructCallback) :
	Model{{}, "Gregory surface " + std::to_string(m_count++)},
	m_surfaceShaderProgram{surfaceShaderProgram},
	m_vectorsShaderProgram{vectorsShaderProgram},
	m_bezierPoints{bezierPoints},
	m_selfDestructCallback{selfDestructCallback}
{
	createPoints();
	updatePos();
	createSurfaceMesh();
	createVectorsMesh();
	registerForNotifications();
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

std::array<glm::vec3, 48> GregorySurface::createVectors() const
{
	std::array<glm::vec3, 48> vectors{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		vectors[16 * patch] = m_points[patch][1];
		vectors[16 * patch + 1] = m_points[patch][6];

		vectors[16 * patch + 2] = m_points[patch][2];
		vectors[16 * patch + 3] = m_points[patch][7];

		vectors[16 * patch + 4] = m_points[patch][9];
		vectors[16 * patch + 5] = m_points[patch][8];
		
		vectors[16 * patch + 6] = m_points[patch][15];
		vectors[16 * patch + 7] = m_points[patch][14];
		
		vectors[16 * patch + 8] = m_points[patch][18];
		vectors[16 * patch + 9] = m_points[patch][13];
		
		vectors[16 * patch + 10] = m_points[patch][17];
		vectors[16 * patch + 11] = m_points[patch][12];
		
		vectors[16 * patch + 12] = m_points[patch][10];
		vectors[16 * patch + 13] = m_points[patch][11];
		
		vectors[16 * patch + 14] = m_points[patch][4];
		vectors[16 * patch + 15] = m_points[patch][5];
	}
	return vectors;
}

void GregorySurface::createSurfaceMesh()
{
	std::array<glm::vec3, 60> points{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		std::copy(m_points[patch].begin(), m_points[patch].end(), points.begin() + 20 * patch);
	}
	m_surfaceMesh = std::make_unique<GregorySurfaceMesh>(points);
}

void GregorySurface::createVectorsMesh()
{
	m_vectorsMesh = std::make_unique<VectorsMesh>(createVectors());
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
	glm::vec3 pos{};
	for (const std::array<glm::vec3, 20>& patch : m_points)
	{
		for (const glm::vec3& point : patch)
		{
			pos += point;
		}
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

void GregorySurface::updateSurfaceMesh()
{
	std::array<glm::vec3, 60> points{};
	for (std::size_t patch = 0; patch < 3; ++patch)
	{
		std::copy(m_points[patch].begin(), m_points[patch].end(), points.begin() + 20 * patch);
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

void GregorySurface::registerForNotifications()
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
}

void GregorySurface::registerForNotifications(Point* point)
{
	m_pointMoveNotifications.push_back(point->registerForMoveNotification
		(
			[this] (void*)
			{
				pointMoveNotification();
			}
		));

	m_pointDestroyNotifications.push_back(point->registerForDestroyNotification
		(
			[this] (void* notification)
			{
				pointDestroyNotification(static_cast<Point::DestroyCallback*>(notification));
			}
		));

	m_pointRereferenceNotifications.push_back(point->registerForRereferenceNotification
		(
			[this] (void* notification, Point* newPoint)
			{
				pointRereferenceNotification(static_cast<Point::RereferenceCallback*>(notification),
					newPoint);
			}
		));
}

void GregorySurface::pointMoveNotification()
{
	updateGeometry();
}

void GregorySurface::pointDestroyNotification(Point::DestroyCallback*)
{
	m_pointMoveNotifications.clear();
	m_pointDestroyNotifications.clear();
	m_pointRereferenceNotifications.clear();
	m_selfDestructCallback(this);
}

void GregorySurface::pointRereferenceNotification(Point::RereferenceCallback* notification,
	Point* newPoint)
{
	auto iterator = std::find_if
	(
		m_pointRereferenceNotifications.begin(), m_pointRereferenceNotifications.end(),
		[notification] (const std::shared_ptr<Point::RereferenceCallback>& sharedNotification)
		{
			return sharedNotification.get() == notification;
		}
	);
	std::size_t notificationIndex = iterator - m_pointRereferenceNotifications.begin();
	std::size_t patchIndex = notificationIndex / 8;
	std::size_t rowIndex = (notificationIndex % 8) / 4;
	std::size_t pointIndex = notificationIndex % 4;

	m_bezierPoints[patchIndex][rowIndex][pointIndex] = newPoint;

	m_pointMoveNotifications[notificationIndex] = newPoint->registerForMoveNotification
		(
			[this] (void*)
			{
				pointMoveNotification();
			}
		);

	m_pointDestroyNotifications[notificationIndex] = newPoint->registerForDestroyNotification
		(
			[this] (void* notification)
			{
				pointDestroyNotification(static_cast<Point::DestroyCallback*>(notification));
			}
		);

	m_pointRereferenceNotifications[notificationIndex] =
		newPoint->registerForRereferenceNotification
		(
			[this] (void* notification, Point* newPoint)
			{
				pointRereferenceNotification(static_cast<Point::RereferenceCallback*>(notification),
					newPoint);
			}
		);

	updateGeometry();
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
