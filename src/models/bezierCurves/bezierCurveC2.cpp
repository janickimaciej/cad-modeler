#include "models/bezierCurves/bezierCurveC2.hpp"

#include "models/bezierCurves/bezierCurve.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <cstddef>
#include <string>

BezierCurveC2::BezierCurveC2(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const ShaderProgram& pointShaderProgram,
	const std::vector<Point*>& points, const SelfDestructCallback& selfDestructCallback,
	std::vector<std::unique_ptr<Point>>& bezierPoints) :
	BezierCurveCX{"BezierCurveC2 " + std::to_string(m_count++), curveShaderProgram,
		polylineShaderProgram, points, selfDestructCallback},
	m_pointShaderProgram{pointShaderProgram}
{
	bezierPoints = createBezierPoints();
	updateBezierPoints();
	createCurveMesh();
	createBezierPolylineMesh();
	registerForNotifications(m_points);
	registerForNotificationsBezier(m_bezierPoints);
}

void BezierCurveC2::render() const
{
	BezierCurve::render();
	if (getRenderPolyline())
	{
		renderBezierPolyline();
	}
}

void BezierCurveC2::addPoints(const std::vector<Point*>& points,
	std::vector<std::unique_ptr<Point>>& bezierPoints)
{
	int newPointCount = 0;
	for (Point* point : points)
	{
		if (std::find(m_points.begin(), m_points.end(), point) == m_points.end())
		{
			m_points.push_back(point);
			registerForNotifications(point);
			++newPointCount;
		}
	}

	int newBezierPointCount = 0;
	if (m_points.size() >= 4)
	{
		newBezierPointCount = 3 * newPointCount;
		if (m_points.size() - newPointCount < 4)
		{
			++newBezierPointCount;
		}
	}

	std::vector<Point*> newBezierPointPtrs{};
	for (int i = 0; i < newBezierPointCount; ++i)
	{
		bezierPoints.push_back(std::make_unique<Point>(m_pointShaderProgram, glm::vec3{}, true));
		m_bezierPoints.push_back(bezierPoints.back().get());
		newBezierPointPtrs.push_back(bezierPoints.back().get());
	}
	updateGeometry();
	registerForNotificationsBezier(newBezierPointPtrs);
}

void BezierCurveC2::deletePoint(int index)
{
	int deletedBezierPointCount = 0;
	if (m_points.size() == 4)
	{
		deletedBezierPointCount = 4;
	}
	else if (m_points.size() > 4)
	{
		deletedBezierPointCount = 3;
	}
	for (int i = deletedBezierPointCount - 1; i >= 0; --i)
	{
		deleteBezierPoint(i);
	}

	BezierCurve::deletePoint(index);
}

int BezierCurveC2::m_count = 0;

void BezierCurveC2::createCurveMesh()
{
	m_curveMesh = std::make_unique<BezierCurveMesh>(pointsToVertices(m_bezierPoints),
		pointsToCurveIndices(m_bezierPoints));
}

std::vector<std::unique_ptr<Point>> BezierCurveC2::createBezierPoints()
{
	std::vector<std::unique_ptr<Point>> bezierPoints{};
	int bezierSegments = static_cast<int>(m_points.size()) - 3;
	if (bezierSegments > 0)
	{
		for (int i = 0; i < 3 * bezierSegments + 1; ++i)
		{
			bezierPoints.push_back(std::make_unique<Point>(m_pointShaderProgram, glm::vec3{},
				true));
			m_bezierPoints.push_back(bezierPoints.back().get());
		}
	}
	return bezierPoints;
}

void BezierCurveC2::createBezierPolylineMesh()
{
	m_bezierPolylineMesh = std::make_unique<PolylineMesh>(pointsToVertices(m_bezierPoints));
}

void BezierCurveC2::updateWithBezierPoint(int index)
{
	if (index == 0)
	{
		m_points[0]->setPos(4.0f * m_bezierPoints[0]->getPos() -
			m_points[1]->getPos() - 2.0f * m_bezierPoints[1]->getPos());
	}
	else if (index == m_bezierPoints.size() - 1)
	{
		std::size_t pointCount = m_points.size();
		std::size_t bezierPointCount = m_bezierPoints.size();
		m_points[pointCount - 1]->setPos(
			4.0f * m_bezierPoints[bezierPointCount - 1]->getPos() -
			m_points[pointCount - 2]->getPos() -
			2.0f * m_bezierPoints[bezierPointCount - 2]->getPos());
	}
	else
	{
		std::size_t bezierSegment = index / 3;
		int remainder = index % 3;
		if (remainder == 0)
		{
			m_points[bezierSegment + 1]->setPos(
				3.0f / 2.0f * m_bezierPoints[index]->getPos() -
				1.0f / 4.0f * (m_points[bezierSegment]->getPos() +
				m_points[bezierSegment + 2]->getPos()));
		}
		else if (remainder == 1)
		{
			m_points[bezierSegment + 1]->setPos(
				3.0f / 2.0f * m_bezierPoints[index]->getPos() -
				1.0f / 2.0f * m_points[bezierSegment + 2]->getPos());
		}
		else
		{
			m_points[bezierSegment + 2]->setPos(
				3.0f / 2.0f * m_bezierPoints[index]->getPos() -
				1.0f / 2.0f * m_points[bezierSegment + 1]->getPos());
		}
	}
	updateGeometry();
}

void BezierCurveC2::updateBezierPoints() const
{
	if (m_bezierPoints.size() == 0)
	{
		return;
	}

	std::size_t bezierSegments = m_points.size() - 3;
	std::vector<glm::vec3> e(bezierSegments + 1);
	std::vector<glm::vec3> f(bezierSegments);
	std::vector<glm::vec3> g(bezierSegments);
	glm::vec3 fLast = (m_points[m_points.size() - 2]->getPos() +
		m_points[m_points.size() - 1]->getPos()) / 2.0f;
	glm::vec3 gFirst = (m_points[0]->getPos() + m_points[1]->getPos()) / 2.0f;

	for (std::size_t i = 0; i < bezierSegments; ++i)
	{
		f[i] = 2.0f / 3.0f * m_points[i + 1]->getPos() +
			1.0f / 3.0f * m_points[i + 2]->getPos();
		g[i] = 1.0f / 3.0f * m_points[i + 1]->getPos() +
			2.0f / 3.0f * m_points[i + 2]->getPos();
	}
	e[0] = (gFirst + f[0]) / 2.0f;
	e[bezierSegments] = (g[bezierSegments - 1] + fLast) / 2.0f;
	for (std::size_t i = 1; i < bezierSegments; ++i)
	{
		e[i] = (g[i - 1] + f[i]) / 2.0f;
	}

	for (std::size_t i = 0; i < bezierSegments; ++i)
	{
		m_bezierPoints[3 * i]->setPos(e[i]);
		m_bezierPoints[3 * i + 1]->setPos(f[i]);
		m_bezierPoints[3 * i + 2]->setPos(g[i]);
	}
	m_bezierPoints[3 * bezierSegments]->setPos(e[bezierSegments]);
}

void BezierCurveC2::updateGeometry()
{
	m_blockNotifications = true;
	updateBezierPoints();
	BezierCurve::updateGeometry();
	updateBezierPolylineMesh();
	m_blockNotifications = false;
}

void BezierCurveC2::updateCurveMesh()
{
	m_curveMesh->update(pointsToVertices(m_bezierPoints), pointsToCurveIndices(m_bezierPoints));
}

void BezierCurveC2::updateBezierPolylineMesh()
{
	m_bezierPolylineMesh->update(pointsToVertices(m_bezierPoints));
}

void BezierCurveC2::renderBezierPolyline() const
{
	usePolylineShaderProgram();
	m_bezierPolylineMesh->render();
}

void BezierCurveC2::deleteBezierPoint(int index)
{
	m_bezierPoints.erase(m_bezierPoints.begin() + index);
	m_bezierPointMoveNotifications.erase(m_bezierPointMoveNotifications.begin() + index);
}

void BezierCurveC2::registerForNotificationsBezier(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotificationsBezier(point);
	}
}

void BezierCurveC2::registerForNotificationsBezier(Point* point)
{
	m_bezierPointMoveNotifications.push_back(point->registerForMoveNotification
	(
		[this] (const Point* point)
		{
			bezierPointMoveNotification(point);
		}
	));
}

void BezierCurveC2::pointMoveNotification()
{
	if (!m_blockNotifications)
	{
		m_blockNotifications = true;
		BezierCurve::pointMoveNotification();
		m_blockNotifications = false;
	}
}

void BezierCurveC2::pointDestroyNotification(const Point* point)
{
	if (!m_blockNotifications)
	{
		m_blockNotifications = true;
		BezierCurve::pointDestroyNotification(point);
		m_blockNotifications = false;
	}
}

void BezierCurveC2::bezierPointMoveNotification(const Point* point)
{
	if (!m_blockNotifications)
	{
		m_blockNotifications = true;
		auto iterator = std::find(m_bezierPoints.begin(), m_bezierPoints.end(), point);
		int index = static_cast<int>(iterator - m_bezierPoints.begin());
		updateWithBezierPoint(index);
		m_blockNotifications = false;
	}
}
