#include "models/bezier_curve_c2.hpp"

#include "models/point.hpp"

#include <cstddef>
#include <iostream>

std::pair<std::unique_ptr<BezierCurveC2>, std::vector<std::unique_ptr<Point>>>
	BezierCurveC2::create(const ShaderProgram& bezierCurveShaderProgram,
	const ShaderProgram& bezierCurvePolylineShaderProgram, const ShaderProgram& pointShaderProgram,
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
		std::unique_ptr<BezierCurveC2>(new BezierCurveC2(bezierCurveShaderProgram,
			bezierCurvePolylineShaderProgram, pointShaderProgram, boorPoints, bezierPointPtrs)),
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
	glm::vec3 fLast = (boorPoints[boorPoints.size() - 2]->getPosition() +
		boorPoints[boorPoints.size() - 1]->getPosition()) / 2.0f;
	glm::vec3 gFirst = (boorPoints[0]->getPosition() + boorPoints[1]->getPosition()) / 2.0f;

	for (std::size_t i = 0; i < bezierSegments; ++i)
	{
		f[i] = 2.0f / 3.0f * boorPoints[i + 1]->getPosition() +
			1.0f / 3.0f * boorPoints[i + 2]->getPosition();
		g[i] = 1.0f / 3.0f * boorPoints[i + 1]->getPosition() +
			2.0f / 3.0f * boorPoints[i + 2]->getPosition();
	}
	e[0] = (gFirst + f[0]) / 2.0f;
	e[bezierSegments] = (g[bezierSegments - 1] + fLast) / 2.0f;
	for (std::size_t i = 1; i < bezierSegments; ++i)
	{
		e[i] = (g[i - 1] + f[i]) / 2.0f;
	}

	for (std::size_t i = 0; i < bezierSegments; ++i)
	{
		bezierPoints[3 * i]->setPosition(e[i]);
		bezierPoints[3 * i + 1]->setPosition(f[i]);
		bezierPoints[3 * i + 2]->setPosition(g[i]);
	}
	bezierPoints[3 * bezierSegments]->setPosition(e[bezierSegments]);
}

void BezierCurveC2::render(RenderMode mode) const
{
	updateShaders(mode);
	renderCurve();
	if (m_renderPolyline)
	{
		renderBoorPolyline();
		renderBezierPolyline();
	}
}

void BezierCurveC2::updateGUI()
{
	m_gui.update();
}

void BezierCurveC2::setPosition(const glm::vec3&)
{ }

void BezierCurveC2::setScreenPosition(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

int BezierCurveC2::getPointCount() const
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
		newBezierPoints.push_back(std::make_unique<Point>(m_pointShaderProgram,
			glm::vec3{}, true));
		m_bezierPoints.push_back(newBezierPoints.back().get());
		newBezierPointPtrs.push_back(newBezierPoints.back().get());
	}
	updateBezierPoints();
	updateMeshes();
	registerForNotificationsBezier(newBezierPointPtrs);
	return newBezierPoints;
}

void BezierCurveC2::deleteBoorPoint(int index)
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

std::vector<std::string> BezierCurveC2::getPointNames() const
{
	std::vector<std::string> pointNames{};
	for (Point* point : m_boorPoints)
	{
		pointNames.push_back(point->getName());
	}
	return pointNames;
}

bool BezierCurveC2::getRenderPolyline() const
{
	return m_renderPolyline;
}

void BezierCurveC2::setRenderPolyline(bool renderPolyline)
{
	m_renderPolyline = renderPolyline;
}

int BezierCurveC2::m_count = 0;

BezierCurveC2::BezierCurveC2(const ShaderProgram& bezierCurveShaderProgram,
	const ShaderProgram& bezierCurvePolylineShaderProgram, const ShaderProgram& pointShaderProgram,
	const std::vector<Point*>& boorPoints, const std::vector<Point*>& bezierPoints) :
	Model{glm::vec3{0, 0, 0}, "BezierCurveC2 " + std::to_string(m_count)},
	m_id{m_count++},
	m_bezierCurveShaderProgram{bezierCurveShaderProgram},
	m_bezierCurvePolylineShaderProgram{bezierCurvePolylineShaderProgram},
	m_pointShaderProgram{pointShaderProgram},
	m_gui{*this},
	m_boorPoints{boorPoints},
	m_bezierPoints{bezierPoints}
{
	updatePosition();
	createCurveMesh();
	createBoorPolylineMesh();
	createBezierPolylineMesh();
	registerForNotificationsBoor(m_boorPoints);
	registerForNotificationsBezier(m_bezierPoints);
}

void BezierCurveC2::createCurveMesh()
{
	glGenBuffers(1, &m_VBOCurve);
	glGenVertexArrays(1, &m_VAOCurve);

	glBindVertexArray(m_VAOCurve);

	updateCurveMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurveC2::createBoorPolylineMesh()
{
	glGenBuffers(1, &m_VBOBoorPolyline);
	glGenVertexArrays(1, &m_VAOBoorPolyline);

	glBindVertexArray(m_VAOBoorPolyline);

	updateBoorPolylineMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurveC2::createBezierPolylineMesh()
{
	glGenBuffers(1, &m_VBOBezierPolyline);
	glGenVertexArrays(1, &m_VAOBezierPolyline);

	glBindVertexArray(m_VAOBezierPolyline);

	updateBezierPolylineMesh();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurveC2::updateShaders(RenderMode) const
{
	m_bezierCurveShaderProgram.use();
	m_bezierCurveShaderProgram.setUniform("isActive", isActive());

	if (m_renderPolyline)
	{
		m_bezierCurvePolylineShaderProgram.use();
		m_bezierCurvePolylineShaderProgram.setUniform("isActive", isActive());
	}
}

void BezierCurveC2::updateWithBezierPoint(int index)
{
	if (index == 0)
	{
		m_boorPoints[0]->setPosition(4.0f * m_bezierPoints[0]->getPosition() -
			m_boorPoints[1]->getPosition() - 2.0f * m_bezierPoints[1]->getPosition());
	}
	else if (index == m_bezierPoints.size() - 1)
	{
		std::size_t boorCount = m_boorPoints.size();
		std::size_t bezierCount = m_bezierPoints.size();
		m_boorPoints[boorCount - 1]->setPosition(
			4.0f * m_bezierPoints[bezierCount - 1]->getPosition() -
			m_boorPoints[boorCount - 2]->getPosition() -
			2.0f * m_bezierPoints[bezierCount - 2]->getPosition());
	}
	else
	{
		std::size_t bezierSegment = index / 3;
		int remainder = index % 3;
		if (remainder == 0)
		{
			m_boorPoints[bezierSegment + 1]->setPosition(
				3.0f / 2.0f * m_bezierPoints[index]->getPosition() -
				1.0f / 4.0f * (m_boorPoints[bezierSegment]->getPosition() +
				m_boorPoints[bezierSegment + 2]->getPosition()));
		}
		else if (remainder == 1)
		{
			m_boorPoints[bezierSegment + 1]->setPosition(
				3.0f / 2.0f * m_bezierPoints[index]->getPosition() -
				1.0f / 2.0f * m_boorPoints[bezierSegment + 2]->getPosition());
		}
		else
		{
			m_boorPoints[bezierSegment + 2]->setPosition(
				3.0f / 2.0f * m_bezierPoints[index]->getPosition() -
				1.0f / 2.0f * m_boorPoints[bezierSegment + 1]->getPosition());
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
	updatePosition();
	updateCurveMesh();
	updateBoorPolylineMesh();
	updateBezierPolylineMesh();
}

void BezierCurveC2::updatePosition()
{
	glm::vec3 position{};
	for (Point* point : m_boorPoints)
	{
		position += point->getPosition();
	}
	m_position = position / static_cast<float>(m_boorPoints.size());
}

void BezierCurveC2::updateCurveMesh()
{
	std::vector<float> vertexData{};

	int remainder = (m_bezierPoints.size() - 1) % 3;
	for (std::size_t i = 0;
		static_cast<int>(i) < static_cast<int>(m_bezierPoints.size()) - remainder - 3; i += 3)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			glm::vec3 position = m_bezierPoints[i + j]->getPosition();
			vertexData.push_back(position.x);
			vertexData.push_back(position.y);
			vertexData.push_back(position.z);
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCurve);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveC2::updateBoorPolylineMesh()
{
	std::vector<float> vertexData{};
	for (const Point* point : m_boorPoints)
	{
		glm::vec3 position = point->getPosition();
		vertexData.push_back(position.x);
		vertexData.push_back(position.y);
		vertexData.push_back(position.z);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOBoorPolyline);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveC2::updateBezierPolylineMesh()
{
	std::vector<float> vertexData{};
	for (const Point* point : m_bezierPoints)
	{
		glm::vec3 position = point->getPosition();
		vertexData.push_back(position.x);
		vertexData.push_back(position.y);
		vertexData.push_back(position.z);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOBezierPolyline);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
		vertexData.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveC2::deleteBezierPoint(int index)
{
	m_bezierPoints.erase(m_bezierPoints.begin() + index);
	m_moveNotificationsBezier.erase(m_moveNotificationsBezier.begin() + index);
}

void BezierCurveC2::registerForNotificationsBoor(Point* point)
{
	m_moveNotificationsBoor.push_back(point->registerForMoveNotification(
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

	m_destroyNotificationsBoor.push_back(point->registerForDestroyNotification(
		[this] (Point* point)
		{
			if (firstCall)
			{
				firstCall = false;
				int index{};
				for (int i = 0; i < m_boorPoints.size(); ++i)
				{
					if (m_boorPoints[i] == point)
					{
						index = i;
						break;
					}
				}
				deleteBoorPoint(index);
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
	m_moveNotificationsBezier.push_back(point->registerForMoveNotification(
		[this] (Point* point)
		{
			if (firstCall)
			{
				firstCall = false;
				int index{};
				for (int i = 0; i < m_bezierPoints.size(); ++i)
				{
					if (m_bezierPoints[i] == point)
					{
						index = i;
						break;
					}
				}
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
		m_bezierCurveShaderProgram.use();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glBindVertexArray(m_VAOCurve);
		glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>((m_bezierPoints.size() - 1) / 3 * 4));
		glBindVertexArray(0);
	}
}

void BezierCurveC2::renderBoorPolyline() const
{
	m_bezierCurvePolylineShaderProgram.use();
	glBindVertexArray(m_VAOBoorPolyline);
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_boorPoints.size()));
	glBindVertexArray(0);
}

void BezierCurveC2::renderBezierPolyline() const
{
	m_bezierCurvePolylineShaderProgram.use();
	glBindVertexArray(m_VAOBezierPolyline);
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_bezierPoints.size()));
	glBindVertexArray(0);
}
