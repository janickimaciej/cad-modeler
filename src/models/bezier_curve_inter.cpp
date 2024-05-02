#include "models/bezier_curve_inter.hpp"

#include "models/point.hpp"

#include <cstddef>

BezierCurveInter::BezierCurveInter(const ShaderProgram& bezierCurveShaderProgram,
	const ShaderProgram& bezierCurvePolylineShaderProgram, const std::vector<Point*>& points) :
	Model{glm::vec3{0, 0, 0}, "BezierCurveInter " + std::to_string(m_count)},
	m_id{m_count++},
	m_bezierCurveShaderProgram{bezierCurveShaderProgram},
	m_bezierCurvePolylineShaderProgram{bezierCurvePolylineShaderProgram},
	m_gui{*this},
	m_points{points}
{
	updatePosition();
	createCurveMesh();
	createPolylineMesh();
	registerForNotifications(m_points);
}

void BezierCurveInter::render(RenderMode mode) const
{
	updateShaders(mode);
	renderCurve();
	if (m_renderPolyline)
	{
		renderPolyline();
	}
}

void BezierCurveInter::updateGUI()
{
	m_gui.update();
}

void BezierCurveInter::setPosition(const glm::vec3&)
{ }

void BezierCurveInter::setScreenPosition(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

int BezierCurveInter::getPointCount() const
{
	return static_cast<int>(m_points.size());
}

void BezierCurveInter::addPoints(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		if (std::find(m_points.begin(), m_points.end(), point) == m_points.end())
		{
			m_points.push_back(point);
		}
	}
	updateGeometry();
	registerForNotifications(points);
}

void BezierCurveInter::deletePoint(int index)
{
	m_points.erase(m_points.begin() + index);
	m_moveNotifications.erase(m_moveNotifications.begin() + index);
	m_destroyNotifications.erase(m_destroyNotifications.begin() + index);
	if (m_points.size() > 0)
	{
		updateGeometry();
	}
}

std::vector<std::string> BezierCurveInter::getPointNames() const
{
	std::vector<std::string> pointNames{};
	for (Point* point : m_points)
	{
		pointNames.push_back(point->getName());
	}
	return pointNames;
}

bool BezierCurveInter::getRenderPolyline() const
{
	return m_renderPolyline;
}

void BezierCurveInter::setRenderPolyline(bool renderPolyline)
{
	m_renderPolyline = renderPolyline;
}

int BezierCurveInter::m_count = 0;

void BezierCurveInter::createCurveMesh()
{
	glGenBuffers(1, &m_VBOCurve);
	glGenVertexArrays(1, &m_VAOCurve);

	glBindVertexArray(m_VAOCurve);

	updateCurveMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurveInter::createPolylineMesh()
{
	glGenBuffers(1, &m_VBOPolyline);
	glGenVertexArrays(1, &m_VAOPolyline);

	glBindVertexArray(m_VAOPolyline);

	updatePolylineMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurveInter::updateShaders(RenderMode) const
{
	m_bezierCurveShaderProgram.use();
	m_bezierCurveShaderProgram.setUniform("isActive", isActive());

	if (m_renderPolyline)
	{
		m_bezierCurvePolylineShaderProgram.use();
		m_bezierCurvePolylineShaderProgram.setUniform("isActive", isActive());
	}
}

void BezierCurveInter::updateGeometry()
{
	updatePosition();
	updateCurveMesh();
	updatePolylineMesh();
}

void BezierCurveInter::updatePosition()
{
	glm::vec3 position{};
	for (Point* point : m_points)
	{
		position += point->getPosition();
	}
	m_position = position / static_cast<float>(m_points.size());
}

void BezierCurveInter::updateCurveMesh()
{
	std::vector<float> vertexData{};

	for (const Point* point : m_points)
	{
		glm::vec3 position = point->getPosition();
		vertexData.push_back(position.x);
		vertexData.push_back(position.y);
		vertexData.push_back(position.z);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCurve);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveInter::updatePolylineMesh()
{
	std::vector<float> vertexData{};
	for (const Point* point : m_points)
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

void BezierCurveInter::registerForNotifications(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		m_moveNotifications.push_back(point->registerForMoveNotification(
			[this] (Point*)
			{
				updateGeometry();
			}
		));

		m_destroyNotifications.push_back(point->registerForDestroyNotification(
			[this, index = static_cast<int>(m_destroyNotifications.size())] (Point*)
			{
				deletePoint(index);
			}
		));
	}
}

void BezierCurveInter::renderCurve() const
{
	if (m_points.size() >= 4)
	{
		m_bezierCurveShaderProgram.use();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glBindVertexArray(m_VAOCurve);
		glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>((m_points.size() - 1) / 3 * 4));
		glBindVertexArray(0);
	}
}

void BezierCurveInter::renderPolyline() const
{
	m_bezierCurvePolylineShaderProgram.use();
	glBindVertexArray(m_VAOPolyline);
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_points.size()));
	glBindVertexArray(0);
}
