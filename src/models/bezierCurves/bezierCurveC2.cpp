#include "models/bezierCurves/bezierCurveC2.hpp"

#include <algorithm>
#include <cstddef>

std::pair<std::unique_ptr<BezierCurveC2>, std::vector<std::unique_ptr<Point>>>
	BezierCurveC2::create(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const ShaderProgram& pointShaderProgram,
	const std::vector<Point*>& boorPoints)
{
	std::vector<std::unique_ptr<Point>> bezierPoints = createBezierPoints(pointShaderProgram,
		boorPoints);

	std::vector<Point*> bezierPointPtrs{};
	for (const std::unique_ptr<Point>& bezierPoint : bezierPoints)
	{
		bezierPointPtrs.push_back(bezierPoint.get());
	}

	return
	{
		std::unique_ptr<BezierCurveC2>(new BezierCurveC2(curveShaderProgram, polylineShaderProgram,
			pointShaderProgram, boorPoints, bezierPointPtrs)),
		std::move(bezierPoints)
	};
}

std::vector<std::unique_ptr<Point>> BezierCurveC2::createBezierPoints(
	const ShaderProgram& pointShaderProgram, const std::vector<Point*> boorPoints)
{
	std::vector<std::unique_ptr<Point>> points{};
	int bezierSegments = static_cast<int>(boorPoints.size()) - 3;
	if (bezierSegments > 0)
	{
		std::vector<Point*> pointPtrs{};
		for (int i = 0; i < 3 * bezierSegments + 1; ++i)
		{
			points.push_back(std::make_unique<Point>(pointShaderProgram, glm::vec3{}, true));
			pointPtrs.push_back(points.back().get());
		}
		updateBezierPoints(pointPtrs, boorPoints);
	}
	return points;
}

void BezierCurveC2::updateBezierPoints(const std::vector<Point*>& bezierPoints,
	const std::vector<Point*> boorPoints)
{
	std::size_t bezierSegments = boorPoints.size() - 3;
	std::vector<glm::vec3> e(bezierSegments + 1);
	std::vector<glm::vec3> f(bezierSegments);
	std::vector<glm::vec3> g(bezierSegments);
	glm::vec3 fLast = (boorPoints[boorPoints.size() - 2]->getPos() +
		boorPoints[boorPoints.size() - 1]->getPos()) / 2.0f;
	glm::vec3 gFirst = (boorPoints[0]->getPos() + boorPoints[1]->getPos()) / 2.0f;

	for (std::size_t i = 0; i < bezierSegments; ++i)
	{
		f[i] = 2.0f / 3.0f * boorPoints[i + 1]->getPos() +
			1.0f / 3.0f * boorPoints[i + 2]->getPos();
		g[i] = 1.0f / 3.0f * boorPoints[i + 1]->getPos() +
			2.0f / 3.0f * boorPoints[i + 2]->getPos();
	}
	e[0] = (gFirst + f[0]) / 2.0f;
	e[bezierSegments] = (g[bezierSegments - 1] + fLast) / 2.0f;
	for (std::size_t i = 1; i < bezierSegments; ++i)
	{
		e[i] = (g[i - 1] + f[i]) / 2.0f;
	}

	for (std::size_t i = 0; i < bezierSegments; ++i)
	{
		bezierPoints[3 * i]->setPos(e[i]);
		bezierPoints[3 * i + 1]->setPos(f[i]);
		bezierPoints[3 * i + 2]->setPos(g[i]);
	}
	bezierPoints[3 * bezierSegments]->setPos(e[bezierSegments]);
}

void BezierCurveC2::render() const
{
	updateShaders();
	renderCurve();
	if (getRenderPolyline())
	{
		renderBoorPolyline();
		renderBezierPolyline();
	}
}

int BezierCurveC2::pointCount() const
{
	return static_cast<int>(m_boorPoints.size());
}

std::vector<std::unique_ptr<Point>> BezierCurveC2::addPoints(const std::vector<Point*>& points)
{
	int newBoorPointCount = 0;
	for (Point* point : points)
	{
		if (std::find(m_boorPoints.begin(), m_boorPoints.end(), point) == m_boorPoints.end())
		{
			m_boorPoints.push_back(point);
			registerForNotificationsBoor(point);
			++newBoorPointCount;
		}
	}

	int newBezierPointCount = 0;
	if (m_boorPoints.size() + newBoorPointCount >= 4)
	{
		newBezierPointCount = 3 * newBoorPointCount;
		if (m_boorPoints.size() < 4)
		{
			++newBezierPointCount;
		}
	}

	std::vector<std::unique_ptr<Point>> newBezierPoints{};
	std::vector<Point*> newBezierPointPtrs{};
	for (int i = 0; i < newBezierPointCount; ++i)
	{
		newBezierPoints.push_back(std::make_unique<Point>(m_pointShaderProgram, glm::vec3{}, true));
		m_bezierPoints.push_back(newBezierPoints.back().get());
		newBezierPointPtrs.push_back(newBezierPoints.back().get());
	}
	updateBezierPoints();
	updateMeshes();
	registerForNotificationsBezier(newBezierPointPtrs);
	return newBezierPoints;
}

void BezierCurveC2::deletePoint(int index)
{
	int deletedBezierPointCount = 0;
	if (m_boorPoints.size() == 4)
	{
		deletedBezierPointCount = 4;
	}
	else if (m_boorPoints.size() > 4)
	{
		deletedBezierPointCount = 3;
	}

	m_boorPoints.erase(m_boorPoints.begin() + index);
	m_moveNotificationsBoor.erase(m_moveNotificationsBoor.begin() + index);
	m_destroyNotificationsBoor.erase(m_destroyNotificationsBoor.begin() + index);
	for (int i = deletedBezierPointCount - 1; i >= 0; --i)
	{
		deleteBezierPoint(i);
	}
	if (m_boorPoints.size() > 0)
	{
		updateBezierPoints();
		updateMeshes();
	}
}

std::string BezierCurveC2::pointName(int index) const
{
	return m_boorPoints[index]->getName();
}

int BezierCurveC2::m_count = 0;

BezierCurveC2::BezierCurveC2(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const ShaderProgram& pointShaderProgram,
	const std::vector<Point*>& boorPoints, const std::vector<Point*>& bezierPoints) :
	BezierCurve{"BezierCurveC2 " + std::to_string(m_count++), curveShaderProgram,
		polylineShaderProgram},
	m_pointShaderProgram{pointShaderProgram},
	m_boorPoints{boorPoints},
	m_bezierPoints{bezierPoints}
{
	updatePos();
	createCurveMesh();
	createBoorPolylineMesh();
	createBezierPolylineMesh();
	registerForNotificationsBoor(m_boorPoints);
	registerForNotificationsBezier(m_bezierPoints);
}

void BezierCurveC2::createCurveMesh()
{
	m_curveMesh = std::make_unique<BezierCurveMesh>(pointsToCurveVertices(m_bezierPoints));
}

void BezierCurveC2::createBoorPolylineMesh()
{
	m_boorPolylineMesh = std::make_unique<PolylineMesh>(pointsToPolylineVertices(m_boorPoints));
}

void BezierCurveC2::createBezierPolylineMesh()
{
	m_bezierPolylineMesh = std::make_unique<PolylineMesh>(pointsToPolylineVertices(m_bezierPoints));
}

void BezierCurveC2::updateWithBezierPoint(int index)
{
	if (index == 0)
	{
		m_boorPoints[0]->setPos(4.0f * m_bezierPoints[0]->getPos() -
			m_boorPoints[1]->getPos() - 2.0f * m_bezierPoints[1]->getPos());
	}
	else if (index == m_bezierPoints.size() - 1)
	{
		std::size_t boorCount = m_boorPoints.size();
		std::size_t bezierCount = m_bezierPoints.size();
		m_boorPoints[boorCount - 1]->setPos(
			4.0f * m_bezierPoints[bezierCount - 1]->getPos() -
			m_boorPoints[boorCount - 2]->getPos() -
			2.0f * m_bezierPoints[bezierCount - 2]->getPos());
	}
	else
	{
		std::size_t bezierSegment = index / 3;
		int remainder = index % 3;
		if (remainder == 0)
		{
			m_boorPoints[bezierSegment + 1]->setPos(
				3.0f / 2.0f * m_bezierPoints[index]->getPos() -
				1.0f / 4.0f * (m_boorPoints[bezierSegment]->getPos() +
				m_boorPoints[bezierSegment + 2]->getPos()));
		}
		else if (remainder == 1)
		{
			m_boorPoints[bezierSegment + 1]->setPos(
				3.0f / 2.0f * m_bezierPoints[index]->getPos() -
				1.0f / 2.0f * m_boorPoints[bezierSegment + 2]->getPos());
		}
		else
		{
			m_boorPoints[bezierSegment + 2]->setPos(
				3.0f / 2.0f * m_bezierPoints[index]->getPos() -
				1.0f / 2.0f * m_boorPoints[bezierSegment + 1]->getPos());
		}
	}
	updateBezierPoints();
	updateMeshes();
}

void BezierCurveC2::updateBezierPoints() const
{
	if (m_boorPoints.size() >= 4)
	{
		updateBezierPoints(m_bezierPoints, m_boorPoints);
	}
}

void BezierCurveC2::updateMeshes()
{
	updatePos();
	updateCurveMesh();
	updateBoorPolylineMesh();
	updateBezierPolylineMesh();
}

void BezierCurveC2::updatePos()
{
	glm::vec3 pos{};
	for (Point* point : m_boorPoints)
	{
		pos += point->getPos();
	}
	m_pos = pos / static_cast<float>(m_boorPoints.size());
}

void BezierCurveC2::updateCurveMesh()
{
	m_curveMesh->update(pointsToCurveVertices(m_bezierPoints));
}

void BezierCurveC2::updateBoorPolylineMesh()
{
	m_boorPolylineMesh->update(pointsToPolylineVertices(m_boorPoints));
}

void BezierCurveC2::updateBezierPolylineMesh()
{
	m_bezierPolylineMesh->update(pointsToPolylineVertices(m_bezierPoints));
}

void BezierCurveC2::deleteBezierPoint(int index)
{
	m_bezierPoints.erase(m_bezierPoints.begin() + index);
	m_moveNotificationsBezier.erase(m_moveNotificationsBezier.begin() + index);
}

void BezierCurveC2::registerForNotificationsBoor(Point* point)
{
	m_moveNotificationsBoor.push_back(point->registerForMoveNotification
	(
		[this] (Point*)
		{
			if (firstCall)
			{
				firstCall = false;
				updateBezierPoints();
				updateMeshes();
				firstCall = true;
			}
		}
	));

	m_destroyNotificationsBoor.push_back(point->registerForDestroyNotification
	(
		[this] (Point* point)
		{
			if (firstCall)
			{
				firstCall = false;
				auto iterator = std::find(m_boorPoints.begin(), m_boorPoints.end(), point);
				int index = static_cast<int>(iterator - m_boorPoints.begin());
				deletePoint(index);
				firstCall = true;
			}
		}
	));
}

void BezierCurveC2::registerForNotificationsBoor(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotificationsBoor(point);
	}
}

void BezierCurveC2::registerForNotificationsBezier(Point* point)
{
	m_moveNotificationsBezier.push_back(point->registerForMoveNotification
	(
		[this] (Point* point)
		{
			if (firstCall)
			{
				firstCall = false;
				auto iterator = std::find(m_bezierPoints.begin(), m_bezierPoints.end(), point);
				int index = static_cast<int>(iterator - m_bezierPoints.begin());
				updateWithBezierPoint(index);
				firstCall = true;
			}
		}
	));
}

void BezierCurveC2::registerForNotificationsBezier(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotificationsBezier(point);
	}
}

void BezierCurveC2::renderCurve() const
{
	if (m_bezierPoints.size() >= 4)
	{
		useCurveShaderProgram();
		m_curveMesh->render();
	}
}

void BezierCurveC2::renderBoorPolyline() const
{
	usePolylineShaderProgram();
	m_boorPolylineMesh->render();
}

void BezierCurveC2::renderBezierPolyline() const
{
	usePolylineShaderProgram();
	m_bezierPolylineMesh->render();
}

std::vector<glm::vec3> BezierCurveC2::pointsToCurveVertices(const std::vector<Point*> points)
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
