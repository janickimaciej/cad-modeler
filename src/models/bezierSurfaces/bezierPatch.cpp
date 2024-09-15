#include "models/bezierSurfaces/bezierPatch.hpp"

#include "models/bezierSurfaces/bezierSurface.hpp"

#include <string>

BezierPatch::BezierPatch(const ShaderProgram& shaderProgram,
	const std::array<std::array<Point*, 4>, 4>& bezierPoints, const BezierSurface& surface,
	bool isOnNegativeUEdge, bool isOnPositiveUEdge, bool isOnNegativeVEdge,
	bool isOnPositiveVEdge) :
	Model{{}, "Bezier patch " + std::to_string(m_count++), false},
	m_shaderProgram{shaderProgram},
	m_bezierPoints{bezierPoints},
	m_surface{surface},
	m_isOnNegativeUEdge{isOnNegativeUEdge},
	m_isOnPositiveUEdge{isOnPositiveUEdge},
	m_isOnNegativeVEdge{isOnNegativeVEdge},
	m_isOnPositiveVEdge{isOnPositiveVEdge}
{
	createSurfaceMesh();
	updatePos();
}

BezierPatch::~BezierPatch()
{
	notifyDestroy();
}

void BezierPatch::render() const
{
	updateShaders();

	m_shaderProgram.use();
	m_shaderProgram.setUniform("orientationFlipped", false);
	m_mesh->render();
	m_shaderProgram.setUniform("orientationFlipped", true);
	m_mesh->render();
}

void BezierPatch::updateGUI()
{
	m_gui.update();
}

void BezierPatch::setPos(const glm::vec3&)
{ }

void BezierPatch::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

void BezierPatch::updatePoints(const std::array<std::array<Point*, 4>, 4>& bezierPoints)
{
	m_bezierPoints = bezierPoints;
	updatePos();
	updateSurfaceMesh();
}

bool BezierPatch::isInvalid() const
{
	return m_isInvalid;
}

void BezierPatch::setInvalid()
{
	m_isInvalid = true;
}

Point* BezierPatch::getCornerPointIfOnEdge(int corner) const
{
	switch (corner)
	{
		case 0:
			if (m_isOnNegativeUEdge || m_isOnNegativeVEdge)
			{
				return m_bezierPoints[0][0];
			}
			break;

		case 1:
			if (m_isOnPositiveUEdge || m_isOnNegativeVEdge)
			{
				return m_bezierPoints[0][3];
			}
			break;

		case 2:
			if (m_isOnPositiveUEdge || m_isOnPositiveVEdge)
			{
				return m_bezierPoints[3][3];
			}
			break;

		case 3:
			if (m_isOnNegativeUEdge || m_isOnPositiveVEdge)
			{
				return m_bezierPoints[3][0];
			}
			break;
	}
	return nullptr;
}

std::array<std::array<Point*, 4>, 2> BezierPatch::getPointsBetweenCorners(int leftCorner,
	int rightCorner) const
{
	std::array<std::array<Point*, 4>, 2> points{};
	for (std::size_t i = 0; i < 2; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			std::size_t u{};
			std::size_t v{};
			switch (leftCorner)
			{
				case 0:
					if (rightCorner == 3)
					{
						u = i;
						v = j;
					}
					else
					{
						u = j;
						v = i;
					}
					break;

				case 1:
					if (rightCorner == 0)
					{
						u = 3 - j;
						v = i;
					}
					else
					{
						u = 3 - i;
						v = j;
					}
					break;

				case 2:
					if (rightCorner == 1)
					{
						u = 3 - i;
						v = 3 - j;
					}
					else
					{
						u = 3 - j;
						v = 3 - i;
					}
					break;

				case 3:
					if (rightCorner == 2)
					{
						u = j;
						v = 3 - i;
					}
					else
					{
						u = i;
						v = 3 - j;
					}
					break;
			}
			points[i][j] = m_bezierPoints[v][u];
		}
	}
	return points;
}

std::shared_ptr<BezierPatch::DestroyCallback> BezierPatch::registerForDestroyNotification(
	const DestroyCallback& callback)
{
	std::shared_ptr<DestroyCallback> notification = std::make_shared<DestroyCallback>(callback);
	m_destroyNotifications.push_back(notification);
	return notification;
}

glm::vec3 BezierPatch::surface(float u, float v) const
{
	std::array<glm::vec3, 4> surfaceV{};
	for (int i = 0; i < 4; ++i)
	{
		surfaceV[i] = deCasteljau(m_bezierPoints[i][0]->getPos(), m_bezierPoints[i][1]->getPos(),
			m_bezierPoints[i][2]->getPos(), m_bezierPoints[i][3]->getPos(), u);
	}
	return deCasteljau(surfaceV[0], surfaceV[1], surfaceV[2], surfaceV[3], v);
}

glm::vec3 BezierPatch::surfaceDU(float u, float v) const
{
	std::array<glm::vec3, 4> surfaceV{};
	for (int i = 0; i < 4; ++i)
	{
		surfaceV[i] = deCasteljauDT(m_bezierPoints[i][0]->getPos(), m_bezierPoints[i][1]->getPos(),
			m_bezierPoints[i][2]->getPos(), m_bezierPoints[i][3]->getPos(), u);
	}
	return deCasteljau(surfaceV[0], surfaceV[1], surfaceV[2], surfaceV[3], v);
}

glm::vec3 BezierPatch::surfaceDV(float u, float v) const
{
	std::array<glm::vec3, 4> surfaceV{};
	for (int i = 0; i < 4; ++i)
	{
		surfaceV[i] = deCasteljau(m_bezierPoints[i][0]->getPos(), m_bezierPoints[i][1]->getPos(),
			m_bezierPoints[i][2]->getPos(), m_bezierPoints[i][3]->getPos(), u);
	}
	return deCasteljauDT(surfaceV[0], surfaceV[1], surfaceV[2], surfaceV[3], v);
}

bool BezierPatch::uWrapped() const
{
	return false;
}

bool BezierPatch::vWrapped() const
{
	return false;
}

int BezierPatch::m_count = 0;

void BezierPatch::createSurfaceMesh()
{
	m_mesh = std::make_unique<BezierPatchMesh>(createVertices());
}

void BezierPatch::updatePos()
{
	glm::vec3 pos{};
	for (const std::array<Point*, 4>& row : m_bezierPoints)
	{
		for (Point* point : row)
		{
			pos += point->getPos();
		}
	}
	m_pos = pos / 16.0f;
}

void BezierPatch::updateSurfaceMesh()
{
	m_mesh->update(createVertices());
}

std::array<glm::vec3, 16> BezierPatch::createVertices()
{
	std::array<glm::vec3, 16> vertices{};
	for (std::size_t v = 0; v < 4; ++v)
	{
		for (std::size_t u = 0; u < 4; ++u)
		{
			vertices[4 * v + u] = m_bezierPoints[v][u]->getPos();
		}
	}
	return vertices;
}

void BezierPatch::updateShaders() const
{
	m_shaderProgram.use();
	m_shaderProgram.setUniform("lineCount", m_surface.getLineCount());
	m_shaderProgram.setUniform("isDark", false);
	m_shaderProgram.setUniform("isSelected", isSelected() || m_surface.isSelected());
}

void BezierPatch::notifyDestroy()
{
	clearExpiredNotifications();

	for (const std::weak_ptr<DestroyCallback>& notification : m_destroyNotifications)
	{
		std::shared_ptr<DestroyCallback> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)();
		}
	}
}

void BezierPatch::clearExpiredNotifications()
{
	std::erase_if
	(
		m_destroyNotifications,
		[] (const std::weak_ptr<DestroyCallback>& notification)
		{
			return notification.expired();
		}
	);
}

glm::vec3 BezierPatch::deCasteljau(const glm::vec3& a, const glm::vec3& b, float t)
{
	return (1 - t) * a + t * b;
}

glm::vec3 BezierPatch::deCasteljau(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	float t)
{
	return (1 - t) * deCasteljau(a, b, t) + t * deCasteljau(b, c, t);
}

glm::vec3 BezierPatch::deCasteljau(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	const glm::vec3& d,	float t)
{
	return (1 - t) * deCasteljau(a, b, c, t) + t * deCasteljau(b, c, d, t);
}

glm::vec3 BezierPatch::deCasteljauDT(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	const glm::vec3& d,	float t)
{
	return deCasteljau(-3.0f * a + 3.0f * b, -3.0f* b + 3.0f * c, -3.0f * c + 3.0f * d, t);
}
