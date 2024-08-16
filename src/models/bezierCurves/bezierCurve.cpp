#include "models/bezierCurves/bezierCurve.hpp"

BezierCurve::BezierCurve(const std::string& name, const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
	const SelfDestructCallback& selfDestructCallback) :
	Model{{}, name},
	m_points{points},
	m_curveShaderProgram{curveShaderProgram},
	m_polylineShaderProgram{polylineShaderProgram},
	m_selfDestructCallback{selfDestructCallback}
{
	updatePos();
	createPolylineMesh();
}

void BezierCurve::render() const
{
	updateShaders();
	renderCurve();
	if (getRenderPolyline())
	{
		renderPolyline();
	}
}

void BezierCurve::updateGUI()
{
	m_gui.update();
}

void BezierCurve::setPos(const glm::vec3&)
{ }

void BezierCurve::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

int BezierCurve::pointCount() const
{
	return static_cast<int>(m_points.size());
}

void BezierCurve::deletePoint(int index)
{
	m_points.erase(m_points.begin() + index);
	m_pointMoveNotifications.erase(m_pointMoveNotifications.begin() + index);
	m_pointDestroyNotifications.erase(m_pointDestroyNotifications.begin() + index);
	if (pointCount() > 0)
	{
		updateGeometry();
	}
	else
	{
		m_selfDestructCallback(this);
	}
}

std::string BezierCurve::pointName(int index) const
{
	return m_points[index]->getName();
}

bool BezierCurve::getRenderPolyline() const
{
	return m_renderPolyline;
}

void BezierCurve::setRenderPolyline(bool renderPolyline)
{
	m_renderPolyline = renderPolyline;
}

void BezierCurve::useCurveShaderProgram() const
{
	m_curveShaderProgram.use();
}

void BezierCurve::usePolylineShaderProgram() const
{
	m_polylineShaderProgram.use();
}

void BezierCurve::updateShaders() const
{
	useCurveShaderProgram();
	m_curveShaderProgram.setUniform("isSelected", isSelected());

	if (getRenderPolyline())
	{
		usePolylineShaderProgram();
		m_polylineShaderProgram.setUniform("isSelected", isSelected());
	}
}

void BezierCurve::createPolylineMesh()
{
	m_polylineMesh = std::make_unique<PolylineMesh>(pointsToVertices(m_points));
}

void BezierCurve::updateGeometry()
{
	updatePos();
	updateCurveMesh();
	updatePolylineMesh();
}

void BezierCurve::updatePos()
{
	glm::vec3 pos{};
	for (Point* point : m_points)
	{
		pos += point->getPos();
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

void BezierCurve::updatePolylineMesh()
{
	m_polylineMesh->update(pointsToVertices(m_points));
}

void BezierCurve::renderPolyline() const
{
	usePolylineShaderProgram();
	m_polylineMesh->render();
}

void BezierCurve::registerForNotifications(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotifications(point);
	}
}

void BezierCurve::registerForNotifications(Point* point)
{
	m_pointMoveNotifications.push_back(point->registerForMoveNotification
	(
		[this] (const Point*)
		{
			pointMoveNotification();
		}
	));

	m_pointDestroyNotifications.push_back(point->registerForDestroyNotification
	(
		[this] (const Point* point)
		{
			pointDestroyNotification(point);
		}
	));
}

void BezierCurve::pointMoveNotification()
{
	updateGeometry();
}

void BezierCurve::pointDestroyNotification(const Point* point)
{
	auto iterator = std::find(m_points.begin(), m_points.end(), point);
	int index = static_cast<int>(iterator - m_points.begin());
	deletePoint(index);
}

std::vector<glm::vec3> BezierCurve::pointsToVertices(const std::vector<Point*> points)
{
	std::vector<glm::vec3> vertices{};
	for (const Point* point : points)
	{
		vertices.push_back(point->getPos());
	}
	return vertices;
}
