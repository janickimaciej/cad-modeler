#include "models/bezierCurveInter.hpp"

#include <algorithm>
#include <cstddef>

BezierCurveInter::BezierCurveInter(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points) :
	Model{{}, "BezierCurveInter " + std::to_string(m_count++)},
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
	m_curveMesh = std::make_unique<BezierCurveInterMesh>(pointsToCurveSegments(m_points));
}

void BezierCurveInter::createPolylineMesh()
{
	m_polylineMesh = std::make_unique<PolylineMesh>(pointsToPolylineVertices(m_points));
}

void BezierCurveInter::updateShaders() const
{
	m_curveShaderProgram.use();
	m_curveShaderProgram.setUniform("isSelected", isSelected());

	if (m_renderPolyline)
	{
		m_polylineShaderProgram.use();
		m_polylineShaderProgram.setUniform("isSelected", isSelected());
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
	m_curveMesh->update(pointsToCurveSegments(m_points));
}

void BezierCurveInter::updatePolylineMesh()
{
	m_polylineMesh->update(pointsToPolylineVertices(m_points));
}

void BezierCurveInter::registerForNotifications(Point* point)
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
		[this] (Point* point)
		{
			auto iterator = std::find(m_points.begin(), m_points.end(), point);
			int index = static_cast<int>(iterator - m_points.begin());
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
		m_curveMesh->render();
	}
}

void BezierCurveInter::renderPolyline() const
{
	m_polylineShaderProgram.use();
	m_polylineMesh->render();
}

std::vector<BezierCurveInterSegmentData> BezierCurveInter::pointsToCurveSegments(
	const std::vector<Point*> points)
{
	std::vector<BezierCurveInterSegmentData> segments{};
	if (points.size() >= 3)
	{
		std::size_t n = points.size() - 1;
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

		for (std::size_t i = 0; i < n; ++i)
		{
			dt[i] = glm::length(points[i + 1]->getPos() - points[i]->getPos());
			a[i] = points[i]->getPos();
		}

		for (std::size_t i = 2; i < n; ++i)
		{
			alpha[i] = dt[i - 1] / (dt[i - 1] + dt[i]);
		}

		for (std::size_t i = 1; i < n - 1; ++i)
		{
			beta[i] = dt[i] / (dt[i - 1] + dt[i]);
		}

		for (std::size_t i = 1; i < n; ++i)
		{
			R[i] = 3.0f * ((points[i + 1]->getPos() - points[i]->getPos()) / dt[i] -
				(points[i]->getPos() - points[i - 1]->getPos()) / dt[i - 1]) / (dt[i - 1] + dt[i]);
		}

		betap[1] = beta[1] / 2;
		for (std::size_t i = 2; i < n - 1; ++i)
		{
			betap[i] = beta[i] / (2 - alpha[i] * betap[i - 1]);
		}

		Rp[1] = R[1] / 2.0f;
		for (std::size_t i = 2; i < n; ++i)
		{
			Rp[i] = (R[i] - alpha[i] * Rp[i - 1]) / (2 - alpha[i] * betap[i - 1]);
		}

		c[n - 1] = Rp[n - 1];
		for (std::size_t i = n - 2; i >= 1; --i)
		{
			c[i] = Rp[i] - betap[i] * c[i + 1];
		}
		c[0] = {0, 0, 0};

		for (std::size_t i = 0; i < n - 1; ++i)
		{
			d[i] = 2.0f * (c[i + 1] - c[i]) / (6 * dt[i]);
		}
		d[n - 1] = -2.0f * c[n - 1] / (6 * dt[n - 1]);

		for (std::size_t i = 0; i < n - 1; ++i)
		{
			b[i] = (a[i + 1] - a[i]) / dt[i] - (c[i] + d[i] * dt[i]) * dt[i];
		}
		b[n - 1] = (points[n]->getPos() - a[n - 1]) / dt[n - 1] -
			(c[n - 1] + d[n - 1] * dt[n - 1]) * dt[n - 1];

		for (std::size_t i = 0; i < n; ++i)
		{
			segments.push_back
			(
				{
					a[i],
					b[i],
					c[i],
					d[i],
					points[i + 1]->getPos(),
					dt[i]
				}
			);
		}
	}
	return segments;
}

std::vector<glm::vec3> BezierCurveInter::pointsToPolylineVertices(const std::vector<Point*> points)
{
	std::vector<glm::vec3> vertices{};
	for (const Point* point : points)
	{
		vertices.push_back(point->getPos());
	}
	return vertices;
}
