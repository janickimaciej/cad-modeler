#include "models/bezierCurveInter.hpp"

#include "models/bezierCurveInterSegmentData.hpp"
#include "models/point.hpp"

#include <cstddef>

BezierCurveInter::BezierCurveInter(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points) :
	Model{glm::vec3{0, 0, 0}, "BezierCurveInter " + std::to_string(m_count)},
	m_id{m_count++},
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

void BezierCurveInter::render() const
{
	updateShaders();
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

void BezierCurveInter::setPos(const glm::vec3&)
{ }

void BezierCurveInter::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
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
			registerForNotifications(point);
		}
	}
	updateGeometry();
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, a)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, b)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, c)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, d)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, nextPoint)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, dt)));
	glEnableVertexAttribArray(5);

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

void BezierCurveInter::updateShaders() const
{
	m_curveShaderProgram.use();
	m_curveShaderProgram.setUniform("isActive", isActive());

	if (m_renderPolyline)
	{
		m_polylineShaderProgram.use();
		m_polylineShaderProgram.setUniform("isActive", isActive());
	}
}

void BezierCurveInter::updateGeometry()
{
	updatePos();
	updateCurveMesh();
	updatePolylineMesh();
}

void BezierCurveInter::updatePos()
{
	glm::vec3 pos{};
	for (Point* point : m_points)
	{
		pos += point->getPos();
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

void BezierCurveInter::updateCurveMesh()
{
	std::vector<BezierCurveInterSegmentData> segmentsData{};

	if (m_points.size() >= 3)
	{
		int n = static_cast<int>(m_points.size()) - 1;
		std::vector<float> dt(n);
		std::vector<glm::vec3> a(n);
		std::vector<glm::vec3> b(n);
		std::vector<glm::vec3> c(n);
		std::vector<glm::vec3> d(n);
		std::vector<float> alpha(n);
		std::vector<float> beta(n);
		std::vector<float> betap(n);
		std::vector<glm::vec3> R(n);
		std::vector<glm::vec3> Rp(n);

		for (int i = 0; i < n; ++i)
		{
			dt[i] = glm::length(m_points[i + 1]->getPos() - m_points[i]->getPos());
			a[i] = m_points[i]->getPos();
		}

		for (int i = 2; i < n; ++i)
		{
			alpha[i] = dt[i - 1] / (dt[i - 1] + dt[i]);
		}

		for (std::size_t i = 1; i < n - 1; ++i)
		{
			beta[i] = dt[i] / (dt[i - 1] + dt[i]);
		}

		for (int i = 1; i < n; ++i)
		{
			R[i] = 3.0f * ((m_points[i + 1]->getPos() - m_points[i]->getPos()) / dt[i] -
				(m_points[i]->getPos() - m_points[i - 1]->getPos()) / dt[i - 1]) /
				(dt[i - 1] + dt[i]);
		}

		betap[1] = beta[1] / 2;
		for (int i = 2; i < n - 1; ++i)
		{
			betap[i] = beta[i] / (2 - alpha[i] * betap[i - 1]);
		}

		Rp[1] = R[1] / 2.0f;
		for (int i = 2; i < n; ++i)
		{
			Rp[i] = (R[i] - alpha[i] * Rp[i - 1]) / (2 - alpha[i] * betap[i - 1]);
		}

		c[n - 1] = Rp[n - 1];
		for (int i = n - 2; i >= 1; --i)
		{
			c[i] = Rp[i] - betap[i] * c[i + 1];
		}
		c[0] = {0, 0, 0};

		for (int i = 0; i < n - 1; ++i)
		{
			d[i] = 2.0f * (c[i + 1] - c[i]) / (6 * dt[i]);
		}
		d[n - 1] = -2.0f * c[n - 1] / (6 * dt[n - 1]);

		for (int i = 0; i < n - 1; ++i)
		{
			b[i] = (a[i + 1] - a[i]) / dt[i] - (c[i] + d[i] * dt[i]) * dt[i];
		}
		b[n - 1] = (m_points[n]->getPos() - a[n - 1]) / dt[n - 1] -
			(c[n - 1] + d[n - 1] * dt[n - 1]) * dt[n - 1];

		for (int i = 0; i < n; ++i)
		{
			segmentsData.push_back(
				{
					a[i],
					b[i],
					c[i],
					d[i],
					m_points[i + 1]->getPos(),
					dt[i]
				}
			);
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCurve);
	glBufferData(GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(segmentsData.size() * sizeof(BezierCurveInterSegmentData)),
		segmentsData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveInter::updatePolylineMesh()
{
	std::vector<float> vertexData{};
	for (const Point* point : m_points)
	{
		glm::vec3 pos = point->getPos();
		vertexData.push_back(pos.x);
		vertexData.push_back(pos.y);
		vertexData.push_back(pos.z);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOPolyline);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveInter::registerForNotifications(Point* point)
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

void BezierCurveInter::registerForNotifications(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotifications(point);
	}
}

void BezierCurveInter::renderCurve() const
{
	if (m_points.size() >= 3)
	{
		m_curveShaderProgram.use();
		glPatchParameteri(GL_PATCH_VERTICES, 1);
		glBindVertexArray(m_VAOCurve);
		glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(m_points.size() - 1));
		glBindVertexArray(0);
	}
}

void BezierCurveInter::renderPolyline() const
{
	m_polylineShaderProgram.use();
	glBindVertexArray(m_VAOPolyline);
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_points.size()));
	glBindVertexArray(0);
}
