#include "models/bezierSurfaces/bezierSurface.hpp"

#include <string>

BezierSurface::BezierSurface(const std::string& name,
	const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV) :
	Model{{}, name},
	m_surfaceShaderProgram{bezierSurfaceShaderProgram},
	m_gridShaderProgram{bezierSurfaceGridShaderProgram},
	m_patchesU{patchesU},
	m_patchesV{patchesV}
{ }

void BezierSurface::render() const
{
	updateShaders();
	renderSurface();
	if (getRenderGrid())
	{
		renderGrid();
	}
}

void BezierSurface::updateGUI()
{
	m_gui.update();
}

void BezierSurface::setPos(const glm::vec3&)
{ }

void BezierSurface::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

bool BezierSurface::getRenderGrid() const
{
	return m_renderGrid;
}

void BezierSurface::setRenderGrid(bool renderGrid)
{
	m_renderGrid = renderGrid;
}

int BezierSurface::getLineCount() const
{
	return m_lineCount;
}

void BezierSurface::setLineCount(int lineCount)
{
	m_lineCount = lineCount;
}

void BezierSurface::updatePos()
{
	glm::vec3 pos{};
	for (Point* point : m_points)
	{
		pos += point->getPos();
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

void BezierSurface::registerForNotifications(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		registerForNotifications(point);
	}
}

std::vector<glm::vec3> BezierSurface::pointsToVertices(const std::vector<Point*> points)
{
	std::vector<glm::vec3> vertices{};
	for (const Point* point : points)
	{
		vertices.push_back(point->getPos());
	}
	return vertices;
}

std::vector<unsigned int> BezierSurface::pointsToSurfaceIndices(const std::vector<Point*> points,
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

std::vector<unsigned int> BezierSurface::pointsToGridIndices(const std::vector<Point*> points,
	int pointsU, int pointsV)
{
	std::vector<unsigned int> indices{};
	for (unsigned int v = 0; v < static_cast<unsigned int>(pointsV); ++v)
	{
		for (unsigned int u = 0; u < static_cast<unsigned int>(pointsU) - 1; ++u)
		{
			indices.push_back(v * pointsU + u);
			indices.push_back(v * pointsU + u + 1);
		}
	}
	for (unsigned int u = 0; u < static_cast<unsigned int>(pointsU); ++u)
	{
		for (unsigned int v = 0; v < static_cast<unsigned int>(pointsV) - 1; ++v)
		{
			indices.push_back(v * pointsU + u);
			indices.push_back((v + 1) * pointsU + u);
		}
	}
	return indices;
}

void BezierSurface::updateShaders() const
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

void BezierSurface::updateGeometry()
{
	updatePos();
	updateSurfaceMesh();
	updateGridMesh();
}

void BezierSurface::renderSurface() const
{
	m_surfaceShaderProgram.use();
	m_surfaceShaderProgram.setUniform("orientationFlipped", false);
	m_surfaceMesh->render();
	m_surfaceShaderProgram.setUniform("orientationFlipped", true);
	m_surfaceMesh->render();
}

void BezierSurface::renderGrid() const
{
	m_gridShaderProgram.use();
	m_gridMesh->render();
}

void BezierSurface::registerForNotifications(Point* point)
{
	m_pointMoveNotifications.push_back(point->registerForMoveNotification
	(
		[this] (const Point*)
		{
			pointMoveNotification();
		}
	));
}

void BezierSurface::pointMoveNotification()
{
	updateGeometry();
}
