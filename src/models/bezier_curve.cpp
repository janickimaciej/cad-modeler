#include "models/bezier_curve.hpp"

#include "models/point.hpp"

#include <cstddef>

BezierCurve::BezierCurve(const Scene& scene, const ShaderProgram& bezierCurveShaderProgram,
	const ShaderProgram& bezierCurvePolylineShaderProgram, const std::vector<Point*>& points) :
	Model{scene, glm::vec3{0, 0, 0}, "BezierCurve " + std::to_string(m_count)},
	m_id{m_count++},
	m_bezierCurveShaderProgram{bezierCurveShaderProgram},
	m_bezierCurvePolylineShaderProgram{bezierCurvePolylineShaderProgram},
	m_gui{*this},
	m_points{points}
{
	updatePosition();
	createMesh();
	createPolylineMesh();
	registerForNotifications(points);
}

void BezierCurve::render(RenderMode mode) const
{
	updateShaders(mode);

	if (m_points.size() >= 4)
	{
		m_bezierCurveShaderProgram.use();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glBindVertexArray(m_VAO);
		int remainder = (m_points.size() - 1) % 3;
		glDrawArrays(GL_PATCHES, 0, (m_points.size() - 1) / 3 * 4);
		glBindVertexArray(0);
	}

	if (m_renderPolyline)
	{
		m_bezierCurvePolylineShaderProgram.use();
		glBindVertexArray(m_VAOPolyline);
		glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_points.size()));
		glBindVertexArray(0);
	}
}

ModelGUI& BezierCurve::getGUI()
{
	return m_gui;
}

void BezierCurve::setPosition(const glm::vec3& position)
{ }

void BezierCurve::setScreenPosition(const glm::vec2& screenPosition)
{ }

int BezierCurve::getPointCount() const
{
	return m_points.size();
}

void BezierCurve::addPoints(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		if (std::find(m_points.begin(), m_points.end(), point) == m_points.end())
		{
			m_points.push_back(point);
		}
	}
	updatePosition();
	updateMesh();
	updatePolylineMesh();
	registerForNotifications(points);
}

void BezierCurve::deletePoint(int index)
{
	m_points.erase(m_points.begin() + index);
	updatePosition();
	updateMesh();
	updatePolylineMesh();
}

std::vector<std::string> BezierCurve::getPointNames() const
{
	std::vector<std::string> pointNames{};
	for (Point* point : m_points)
	{
		pointNames.push_back(point->getName());
	}
	return pointNames;
}

bool BezierCurve::getRenderPolyline() const
{
	return m_renderPolyline;
}

void BezierCurve::setRenderPolyline(bool renderPolyline)
{
	m_renderPolyline = renderPolyline;
}

int BezierCurve::m_count = 0;

void BezierCurve::createMesh()
{
	glGenBuffers(1, &m_VBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	updateMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurve::createPolylineMesh()
{
	glGenBuffers(1, &m_VBOPolyline);
	glGenVertexArrays(1, &m_VAOPolyline);

	glBindVertexArray(m_VAOPolyline);

	updatePolylineMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurve::updateShaders(RenderMode renderMode) const
{
	m_bezierCurveShaderProgram.use();
	m_bezierCurveShaderProgram.setUniform1b("isActive", isActive());

	if (m_renderPolyline)
	{
		m_bezierCurvePolylineShaderProgram.use();
		m_bezierCurvePolylineShaderProgram.setUniform1b("isActive", isActive());
	}
}

void BezierCurve::updatePosition()
{
	glm::vec3 position{};
	for (Point* point : m_points)
	{
		position += point->getPosition();
	}
	m_position = position / static_cast<float>(m_points.size());
}

void BezierCurve::updateMesh()
{
	std::vector<float> vertexData{};

	int remainder = (m_points.size() - 1) % 3;
	for (std::size_t i = 0; static_cast<int>(i) < static_cast<int>(m_points.size()) - remainder - 3;
		i += 3)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			glm::vec3 position = m_points[i + j]->getPosition();
			vertexData.push_back(position.x);
			vertexData.push_back(position.y);
			vertexData.push_back(position.z);
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurve::updatePolylineMesh()
{
	std::vector<float> vertexData{};
	for (Point* point : m_points)
	{
		glm::vec3 position = point->getPosition();
		vertexData.push_back(position.x);
		vertexData.push_back(position.y);
		vertexData.push_back(position.z);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOPolyline);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurve::registerForNotifications(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		m_notifications.push_back(point->registerForMoveNotification(
			[this] (Point* movedPoint)
			{
				updatePosition();
				updateMesh();
				updatePolylineMesh();
			}
		));

		m_notifications.push_back(point->registerForDestroyNotification(
			[this] (Point* deletedPoint)
			{
				std::erase_if(m_points,
					[deletedPoint] (Point* point)
					{
						return point == deletedPoint;
					}
				);
				if (m_points.size() > 0)
				{
					updatePosition();
					updateMesh();
					updatePolylineMesh();
				}
			}
		));
	}
}
