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
	BezierCurveCX{"C2 Bezier Curve " + std::to_string(m_count++), curveShaderProgram,
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
	int oldPointCount = static_cast<int>(m_points.size());
	for (Point* point : points)
	{
		if (std::find(m_points.begin(), m_points.end(), point) == m_points.end())
		{
			m_points.push_back(point);
			registerForNotifications(point);
		}
	}

	int newBezierPointCount = 0;
	if (m_points.size() >= 5)
	{
		newBezierPointCount = 3 * (static_cast<int>(m_points.size()) - std::max(oldPointCount, 4));
	}

	std::vector<Point*> newBezierPointPtrs{};
	for (int i = 0; i < newBezierPointCount; ++i)
	{
		bezierPoints.push_back(std::make_unique<Point>(m_pointShaderProgram, glm::vec3{}, false,
			true));
		m_bezierPoints.push_back(bezierPoints.back().get());
		newBezierPointPtrs.push_back(bezierPoints.back().get());
	}
	updateGeometry();
	registerForNotificationsBezier(newBezierPointPtrs);
}

void BezierCurveC2::deletePoint(int index)
{
	if (m_points.size() >= 5)
	{
		for (int i = 0; i < 3; ++i)
		{
			deleteBezierPoint(0);
		}
	}

	BezierCurve::deletePoint(index);
}

int BezierCurveC2::m_count = 0;

void BezierCurveC2::createCurveMesh()
{
	std::vector<Point*> allBezierPoints = getAllBezierPoints();
	m_curveMesh = std::make_unique<BezierCurveMesh>(createVertices(allBezierPoints),
		createCurveIndices(allBezierPoints));
}

std::vector<std::unique_ptr<Point>> BezierCurveC2::createBezierPoints()
{
	std::vector<std::unique_ptr<Point>> bezierPoints{};
	int bezierSegments = static_cast<int>(m_points.size()) - 3;
	if (bezierSegments > 0)
	{
		for (int i = 0; i < 3 * bezierSegments - 3; ++i)
		{
			bezierPoints.push_back(std::make_unique<Point>(m_pointShaderProgram, glm::vec3{}, false,
				true));
			m_bezierPoints.push_back(bezierPoints.back().get());
		}
	}
	return bezierPoints;
}

void BezierCurveC2::createBezierPolylineMesh()
{
	m_bezierPolylineMesh = std::make_unique<PolylineMesh>(createVertices(getAllBezierPoints()));
}

std::vector<Point*> BezierCurveC2::getAllBezierPoints() const
{
	std::vector<Point*> allBezierPoints{};
	if (m_points.size() >= 4)
	{
		allBezierPoints.push_back(m_points[0]);
		allBezierPoints.push_back(m_points[1]);
		allBezierPoints.insert(allBezierPoints.end(), m_bezierPoints.begin(), m_bezierPoints.end());
		allBezierPoints.push_back(m_points[m_points.size() - 2]);
		allBezierPoints.push_back(m_points[m_points.size() - 1]);
	}
	return allBezierPoints;
}

void BezierCurveC2::updateWithBezierPoint(int index)
{
	if (index == 0)
	{
		m_points[2]->setPos(2.0f * m_bezierPoints[index]->getPos() - m_points[1]->getPos());
	}
	else if (index == 1 && index == m_bezierPoints.size() - 2)
	{
		m_points[2]->setPos(2.0f * m_bezierPoints[index]->getPos() -
			(m_points[1]->getPos() + m_points[3]->getPos()) / 2.0f);
	}
	else if (index == 1)
	{
		m_points[2]->setPos((12.0f * m_bezierPoints[index]->getPos() -
			3.0f * m_points[1]->getPos() - 2.0f * m_points[3]->getPos()) / 7.0f);
	}
	else if (index == m_bezierPoints.size() - 2)
	{
		m_points[m_points.size() - 3]->setPos((12.0f * m_bezierPoints[index]->getPos() -
			3.0f * m_points[m_points.size() - 2]->getPos() -
			2.0f * m_points[m_points.size() - 4]->getPos()) / 7.0f);
	}
	else if (index == m_bezierPoints.size() - 1)
	{
		m_points[m_points.size() - 3]->setPos(2.0f * m_bezierPoints[index]->getPos() -
			m_points[m_points.size() - 2]->getPos());
	}
	else
	{
		std::size_t allBezierPointsIndex = static_cast<std::size_t>(index) + 2;
		std::size_t bezierSegment = allBezierPointsIndex / 3;
		int remainder = allBezierPointsIndex % 3;
		switch (remainder)
		{
			case 0:
				m_points[bezierSegment + 1]->setPos(3.0f / 2.0f * m_bezierPoints[index]->getPos() -
				(m_points[bezierSegment]->getPos() + m_points[bezierSegment + 2]->getPos()) / 4.0f);
				break;

			case 1:
				m_points[bezierSegment + 1]->setPos((3.0f * m_bezierPoints[index]->getPos() -
					m_points[bezierSegment + 2]->getPos()) / 2.0f);
				break;

			case 2:
				m_points[bezierSegment + 2]->setPos((3.0f * m_bezierPoints[index]->getPos() -
					m_points[bezierSegment + 1]->getPos()) / 2.0f);
				break;
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

	f[0] = m_points[1]->getPos();
	g[0] = (m_points[1]->getPos() + m_points[2]->getPos()) / 2.0f;
	for (std::size_t i = 1; i < bezierSegments - 1; ++i)
	{
		f[i] = (2.0f * m_points[i + 1]->getPos() + m_points[i + 2]->getPos()) / 3.0f;
		g[i] = (m_points[i + 1]->getPos() + 2.0f * m_points[i + 2]->getPos()) / 3.0f;
	}
	f[bezierSegments - 1] =
		(m_points[bezierSegments]->getPos() + m_points[bezierSegments + 1]->getPos()) / 2.0f;
	g[bezierSegments - 1] = m_points[bezierSegments + 1]->getPos();

	e[0] = m_points[0]->getPos();
	for (std::size_t i = 1; i < bezierSegments; ++i)
	{
		e[i] = (g[i - 1] + f[i]) / 2.0f;
	}
	e[bezierSegments] = m_points[bezierSegments + 2]->getPos();

	m_bezierPoints[0]->setPos(g[0]);
	for (std::size_t i = 1; i < bezierSegments - 1; ++i)
	{
		m_bezierPoints[3 * i - 2]->setPos(e[i]);
		m_bezierPoints[3 * i - 1]->setPos(f[i]);
		m_bezierPoints[3 * i]->setPos(g[i]);
	}
	m_bezierPoints[3 * bezierSegments - 5]->setPos(e[bezierSegments - 1]);
	m_bezierPoints[3 * bezierSegments - 4]->setPos(f[bezierSegments - 1]);
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
	if (m_points.size() >= 4)
	{
		std::vector<Point*> allBezierPoints = getAllBezierPoints();
		m_curveMesh->update(createVertices(allBezierPoints),
			createCurveIndices(allBezierPoints));
	}
}

void BezierCurveC2::updateBezierPolylineMesh()
{
	m_bezierPolylineMesh->update(createVertices(getAllBezierPoints()));
}

void BezierCurveC2::renderBezierPolyline() const
{
	if (m_points.size() >= 4)
	{
		usePolylineShaderProgram();
		m_bezierPolylineMesh->render();
	}
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
