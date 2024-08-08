#include "models/bezierCurveC0.hpp"

#include <cstddef>

BezierCurveC0::BezierCurveC0(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points) :
	Model{{}, "BezierCurveC0 " + std::to_string(m_count++)},
	m_curveShaderProgram{curveShaderProgram},
	m_polylineShaderProgram{polylineShaderProgram},
	m_gui{*this},
	m_points{points}
{
	updatePos();
	createCurveMesh();
	createPolylineMesh();
	registerForNotifications(m_points);
}

void BezierCurveC0::render() const
{
	updateShaders();
	renderCurve();
	if (m_renderPolyline)
	{
		renderPolyline();
	}
}

void BezierCurveC0::updateGUI()
{
	m_gui.update();
}

void BezierCurveC0::setPos(const glm::vec3&)
{ }

void BezierCurveC0::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

int BezierCurveC0::getPointCount() const
{
	return static_cast<int>(m_points.size());
}

void BezierCurveC0::addPoints(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		if (std::find(m_points.begin(), m_points.end(), point) == m_points.end())
		{
			m_points.push_back(point);
			registerForNotifications(point);
		}
	}
	updateGeometry();
}

void BezierCurveC0::deletePoint(int index)
{
	m_points.erase(m_points.begin() + index);
	m_moveNotifications.erase(m_moveNotifications.begin() + index);
	m_destroyNotifications.erase(m_destroyNotifications.begin() + index);
	if (m_points.size() > 0)
	{
		updateGeometry();
	}
}

std::vector<std::string> BezierCurveC0::getPointNames() const
{
	std::vector<std::string> pointNames{};
	for (Point* point : m_points)
	{
		pointNames.push_back(point->getName());
	}
	return pointNames;
}

bool BezierCurveC0::getRenderPolyline() const
{
	return m_renderPolyline;
}

void BezierCurveC0::setRenderPolyline(bool renderPolyline)
{
	m_renderPolyline = renderPolyline;
}

int BezierCurveC0::m_count = 0;

void BezierCurveC0::createCurveMesh()
{
	glGenBuffers(1, &m_VBOCurve);
	glGenVertexArrays(1, &m_VAOCurve);

	glBindVertexArray(m_VAOCurve);

	updateCurveMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurveC0::createPolylineMesh()
{
	m_polylineMesh = std::make_unique<PolylineMesh>(pointsToVertices(m_points));
}

void BezierCurveC0::updateShaders() const
{
	m_curveShaderProgram.use();
	m_curveShaderProgram.setUniform("isSelected", isSelected());

	if (m_renderPolyline)
	{
		m_polylineShaderProgram.use();
		m_polylineShaderProgram.setUniform("isSelected", isSelected());
	}
}

void BezierCurveC0::updateGeometry()
{
	updatePos();
	updateCurveMesh();
	updatePolylineMesh();
}

void BezierCurveC0::updatePos()
{
	glm::vec3 pos{};
	for (Point* point : m_points)
	{
		pos += point->getPos();
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

void BezierCurveC0::updateCurveMesh()
{
	std::vector<float> vertexData{};

	int remainder = (m_points.size() - 1) % 3;
	int patchCount = (m_points.size() - 1) / 3;
	for (std::size_t i = 0; static_cast<int>(i) < patchCount; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			glm::vec3 pos = m_points[3 * i + j]->getPos();
			vertexData.push_back(pos.x);
			vertexData.push_back(pos.y);
			vertexData.push_back(pos.z);
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCurve);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveC0::updatePolylineMesh()
{
	m_polylineMesh->update(pointsToVertices(m_points));
}

void BezierCurveC0::registerForNotifications(Point* point)
{
	m_moveNotifications.push_back(point->registerForMoveNotification
	(
		[this] (Point*)
		{
			updateGeometry();
		}
	));

	m_destroyNotifications.push_back(point->registerForDestroyNotification
	(
		[this, index = static_cast<int>(m_destroyNotifications.size())] (Point*)
		{
			deletePoint(index);
		}
	));
}

void BezierCurveC0::registerForNotifications(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotifications(point);
	}
}

void BezierCurveC0::renderCurve() const
{
	if (m_points.size() >= 4)
	{
		m_curveShaderProgram.use();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glBindVertexArray(m_VAOCurve);
		glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>((m_points.size() - 1) / 3 * 4));
		glBindVertexArray(0);
	}
}

void BezierCurveC0::renderPolyline() const
{
	m_polylineShaderProgram.use();
	m_polylineMesh->render();
}

std::vector<glm::vec3> BezierCurveC0::pointsToVertices(const std::vector<Point*> points)
{
	std::vector<glm::vec3> vertices{};
	for (const Point* point : points)
	{
		vertices.push_back(point->getPos());
	}
	return vertices;
}
