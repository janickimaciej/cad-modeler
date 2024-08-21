#include "models/bezierSurfaces/bezierSurfaceC0.hpp"

#include <string>

BezierSurfaceC0::BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, const ShaderProgram& pointShaderProgram,
	int patchesU, int patchesV, const glm::vec3& pos, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping, std::vector<std::unique_ptr<Point>>& bezierPoints) :
	Model{{}, "BezierSurfaceC0 " + std::to_string(m_count++)},
	m_surfaceShaderProgram{bezierSurfaceShaderProgram},
	m_gridShaderProgram{bezierSurfaceGridShaderProgram},
	m_patchesU{patchesU},
	m_patchesV{patchesV}
{
	bezierPoints = createBezierPoints(pointShaderProgram, pos, sizeU, sizeV);
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
	m_gui.update();
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

int BezierSurfaceC0::getLineCount() const
{
	return m_lineCount;
}

void BezierSurfaceC0::setLineCount(int lineCount)
{
	m_lineCount = lineCount;
}

int BezierSurfaceC0::m_count = 0;

void BezierSurfaceC0::updateShaders() const
{
	m_surfaceShaderProgram.use();
	m_surfaceShaderProgram.setUniform("lineCount", m_lineCount);
	m_surfaceShaderProgram.setUniform("isDark", false);
	m_surfaceShaderProgram.setUniform("isSelected", isSelected());

	if (m_renderGrid)
	{
		m_gridShaderProgram.use();
		m_gridShaderProgram.setUniform("modelMatrix", m_modelMatrix);
		m_gridShaderProgram.setUniform("isDark", true);
		m_gridShaderProgram.setUniform("isSelected", isSelected());
	}
}

void BezierSurfaceC0::createSurfaceMesh()
{
	m_surfaceMesh = std::make_unique<BezierSurfaceMesh>(pointsToVertices(m_points),
		pointsToSurfaceIndices(m_points, m_patchesU, m_patchesV));
}

std::vector<std::unique_ptr<Point>> BezierSurfaceC0::createBezierPoints(
	const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU, float sizeV)
{
	int pointsU = 3 * m_patchesU + 1;
	int pointsV = 3 * m_patchesV + 1;
	float dU = sizeU / (3 * m_patchesU);
	float dV = sizeV / (3 * m_patchesV);
	glm::vec3 startCorner = pos + glm::vec3{-sizeU / 2, 0, -sizeV / 2};
	std::vector<std::unique_ptr<Point>> bezierPoints{};
	for (int v = 0; v < pointsV; ++v)
	{
		for (int u = 0; u < pointsU; ++u)
		{
			glm::vec3 pointPos = startCorner + glm::vec3{u * dU, 0, v * dV};
			bezierPoints.push_back(std::make_unique<Point>(pointShaderProgram, pointPos, true));
			m_points.push_back(bezierPoints.back().get());
		}
	}
	return bezierPoints;
}

void BezierSurfaceC0::createGridMesh()
{
	m_gridMesh = std::make_unique<Mesh>(pointsToVertices(m_points),
		pointsToGridIndices(m_points, m_patchesU, m_patchesV));
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
	m_surfaceMesh->update(pointsToVertices(m_points), pointsToSurfaceIndices(m_points, m_patchesU,
		m_patchesV));
}

void BezierSurfaceC0::updateGridMesh()
{
	m_gridMesh->update(pointsToVertices(m_points), pointsToGridIndices(m_points, m_patchesU,
		m_patchesV));
}

void BezierSurfaceC0::renderSurface() const
{
	m_surfaceShaderProgram.use();
	m_surfaceShaderProgram.setUniform("orientationFlipped", false);
	m_surfaceMesh->render();
	m_surfaceShaderProgram.setUniform("orientationFlipped", true);
	m_surfaceMesh->render();
}

void BezierSurfaceC0::renderGrid() const
{
	m_gridShaderProgram.use();
	m_gridMesh->render();
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

std::vector<unsigned int> BezierSurfaceC0::pointsToSurfaceIndices(const std::vector<Point*> points,
	int patchesU, int patchesV)
{
	unsigned int pointsU = 3 * patchesU + 1;
	std::vector<unsigned int> indices{};
	for (unsigned int patchV = 0; patchV < static_cast<unsigned int>(patchesV); ++patchV)
	{
		for (unsigned int patchU = 0; patchU < static_cast<unsigned int>(patchesU); ++patchU)
		{
			for (unsigned int v = 3 * patchV; v < 3 * patchV + 4; ++v)
			{
				for (unsigned int u = 3 * patchU; u < 3 * patchU + 4; ++u)
				{
					indices.push_back(v * pointsU + u);
				}
			}
		}
	}
	return indices;
}

std::vector<unsigned int> BezierSurfaceC0::pointsToGridIndices(const std::vector<Point*> points,
	int patchesU, int patchesV)
{
	unsigned int pointsU = 3 * patchesU + 1;
	unsigned int pointsV = 3 * patchesV + 1;
	std::vector<unsigned int> indices{};
	for (unsigned int v = 0; v < pointsV; ++v)
	{
		for (unsigned int u = 0; u < pointsU - 1; ++u)
		{
			indices.push_back(v * pointsU + u);
			indices.push_back(v * pointsU + u + 1);
		}
	}
	for (unsigned int u = 0; u < pointsU; ++u)
	{
		for (unsigned int v = 0; v < pointsV - 1; ++v)
		{
			indices.push_back(v * pointsU + u);
			indices.push_back((v + 1) * pointsU + u);
		}
	}
	return indices;
}
