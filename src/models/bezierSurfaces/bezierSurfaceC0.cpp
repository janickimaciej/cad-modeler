#include "models/bezierSurfaces/bezierSurfaceC0.hpp"

#include <string>

BezierSurfaceC0::BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV,
	std::vector<std::unique_ptr<Point>>& bezierPoints) :
	Model{{}, "BezierSurfaceC0 " + std::to_string(m_count++)},
	m_surfaceShaderProgram{bezierSurfaceShaderProgram},
	m_gridShaderProgram{bezierSurfaceGridShaderProgram}
{
	bezierPoints = createBezierPoints();
	updatePos();
	createSurfaceMesh();
	createGridMesh();
	registerForNotifications(m_points);
}

void BezierSurfaceC0::render() const
{
	updateShaders();
	renderSurface();
	if (getRenderGrid())
	{
		renderGrid();
	}
}

void BezierSurfaceC0::updateGUI()
{
	// m_gui.update();
}

void BezierSurfaceC0::setPos(const glm::vec3&)
{ }

void BezierSurfaceC0::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

bool BezierSurfaceC0::getRenderGrid() const
{
	return m_renderGrid;
}

void BezierSurfaceC0::setRenderGrid(bool renderGrid)
{
	m_renderGrid = renderGrid;
}

void BezierSurfaceC0::updateShaders() const
{
	// TODO
}

void BezierSurfaceC0::createSurfaceMesh()
{
	// m_surfaceMesh = std::make_unique<BezierSurfaceMesh>(pointsToVertices(m_points),
		// pointsToSurfaceIndices(m_points));
}

std::vector<std::unique_ptr<Point>> BezierSurfaceC0::createBezierPoints()
{
	// TODO
}

void BezierSurfaceC0::createGridMesh()
{
	// m_gridMesh = std::make_unique<GridMesh>(pointsToVertices(m_points));
}

void BezierSurfaceC0::updateGeometry()
{
	updatePos();
	updateSurfaceMesh();
	updateGridMesh();
}

void BezierSurfaceC0::updatePos()
{
	glm::vec3 pos{};
	for (Point* point : m_points)
	{
		pos += point->getPos();
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

void BezierSurfaceC0::updateSurfaceMesh()
{
	// m_surfaceMesh->update(pointsToVertices(m_points));
}

void BezierSurfaceC0::updateGridMesh()
{
	// m_gridMesh->update(pointsToVertices(m_points));
}

void BezierSurfaceC0::renderSurface() const
{
	m_surfaceShaderProgram.use();
	// m_surfaceMesh->render();
}

void BezierSurfaceC0::renderGrid() const
{
	m_gridShaderProgram.use();
	// m_gridMesh.render();
}
	
void BezierSurfaceC0::registerForNotifications(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotifications(point);
	}
}

void BezierSurfaceC0::registerForNotifications(Point* point)
{
	m_pointMoveNotifications.push_back(point->registerForMoveNotification
	(
		[this] (const Point*)
		{
			pointMoveNotification();
		}
	));
}

void BezierSurfaceC0::pointMoveNotification()
{
	updateGeometry();
}

std::vector<glm::vec3> BezierSurfaceC0::pointsToVertices(const std::vector<Point*> points)
{
	std::vector<glm::vec3> vertices{};
	for (const Point* point : points)
	{
		vertices.push_back(point->getPos());
	}
	return vertices;
}

std::vector<unsigned int> BezierSurfaceC0::pointsToCurveIndices(const std::vector<Point*> points)
{
	// TODO
}
