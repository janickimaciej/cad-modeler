#include "models/bezierCurves/bezierCurveC0.hpp"

#include <algorithm>
#include <cstddef>

BezierCurveC0::BezierCurveC0(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points) :
	BezierCurve{"BezierCurveC0 " + std::to_string(m_count++), curveShaderProgram,
		polylineShaderProgram},
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
	if (getRenderPolyline())
	{
		renderPolyline();
	}
}

int BezierCurveC0::pointCount() const
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

std::string BezierCurveC0::pointName(int index) const
{
	return m_points[index]->getName();
}

int BezierCurveC0::m_count = 0;

void BezierCurveC0::createCurveMesh()
{
	m_curveMesh = std::make_unique<BezierCurveMesh>(pointsToCurveVertices(m_points));
}

void BezierCurveC0::createPolylineMesh()
{
	m_polylineMesh = std::make_unique<PolylineMesh>(pointsToPolylineVertices(m_points));
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
	m_curveMesh->update(pointsToCurveVertices(m_points));
}

void BezierCurveC0::updatePolylineMesh()
{
	m_polylineMesh->update(pointsToPolylineVertices(m_points));
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
		[this] (Point* point)
		{
			auto iterator = std::find(m_points.begin(), m_points.end(), point);
			int index = static_cast<int>(iterator - m_points.begin());
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
		useCurveShaderProgram();
		m_curveMesh->render();
	}
}

void BezierCurveC0::renderPolyline() const
{
	usePolylineShaderProgram();
	m_polylineMesh->render();
}

std::vector<glm::vec3> BezierCurveC0::pointsToCurveVertices(const std::vector<Point*> points)
{
	if (points.size() == 0)
	{
		return {};
	}

	std::vector<glm::vec3> vertices{};
	std::size_t patchCount = (points.size() - 1) / 3;
	for (std::size_t i = 0; i < patchCount; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			vertices.push_back(points[3 * i + j]->getPos());
		}
	}
	return vertices;
}
