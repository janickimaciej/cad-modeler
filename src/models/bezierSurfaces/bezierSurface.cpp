#include "models/bezierSurfaces/bezierSurface.hpp"

#include <glm/gtc/constants.hpp>

#include <string>

BezierSurface::BezierSurface(const std::string& name,
	const ShaderProgram& bezierSurfaceShaderProgram,
	const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV,
	BezierSurfaceWrapping wrapping) :
	Model{{}, name},
	m_surfaceShaderProgram{bezierSurfaceShaderProgram},
	m_gridShaderProgram{bezierSurfaceGridShaderProgram},
	m_patchesU{static_cast<std::size_t>(patchesU)},
	m_patchesV{static_cast<std::size_t>(patchesV)},
	m_wrapping{wrapping}
{ }

BezierSurface::~BezierSurface()
{
	m_pointMoveNotifications.clear();
	m_pointDeletabilityLocks.clear();
	for (const std::vector<Point*>& row : m_points)
	{
		for (Point* point : row)
		{
			if (point->isReferenced())
			{
				point->tryMakeDeletable();
			}
		}
	}
}

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
	for (const std::vector<Point*>& row : m_points)
	{
		for (const Point* point : row)
		{
			pos += point->getPos();
		}
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

void BezierSurface::registerForNotifications()
{
	for (const std::vector<Point*>& row : m_points)
	{
		for (Point* point : row)
		{
			registerForNotifications(point);
		}
	}
}

std::vector<std::vector<glm::vec3>> BezierSurface::createBoorPoints(const glm::vec3& pos,
	float sizeU, float sizeV) const
{
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			return createBoorPointsNoWrapping(pos, sizeU, sizeV);
			
		case BezierSurfaceWrapping::u:
			return createBoorPointsUWrapping(pos, sizeU, sizeV);
			
		case BezierSurfaceWrapping::v:
			return createBoorPointsVWrapping(pos, sizeU, sizeV);
	}
	return {};
}

std::vector<std::vector<glm::vec3>> BezierSurface::createBezierPoints(
	const std::vector<std::vector<glm::vec3>>& boorPoints) const
{
	std::size_t bezierPointsU{};
	std::size_t bezierPointsV{};
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			bezierPointsU = 3 * m_patchesU + 1;
			bezierPointsV = 3 * m_patchesV + 1;
			break;

		case BezierSurfaceWrapping::u:
			bezierPointsU = 3 * m_patchesU;
			bezierPointsV = 3 * m_patchesV + 1;
			break;

		case BezierSurfaceWrapping::v:
			bezierPointsU = 3 * m_patchesU + 1;
			bezierPointsV = 3 * m_patchesV;
			break;
	}

	std::vector<std::vector<glm::vec3>> bezierPoints(bezierPointsV);
	for (std::size_t v = 0; v < bezierPoints.size(); ++v)
	{
		bezierPoints[v].resize(bezierPointsU);
	}

	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			createBezierPointsNoWrapping(boorPoints, bezierPoints);
			break;

		case BezierSurfaceWrapping::u:
			createBezierPointsUWrapping(boorPoints, bezierPoints);
			break;

		case BezierSurfaceWrapping::v:
			createBezierPointsVWrapping(boorPoints, bezierPoints);
			break;
	}

	return bezierPoints;
}

std::vector<glm::vec3> BezierSurface::createVertices(const std::vector<std::vector<Point*>>& points)
{
	std::vector<glm::vec3> vertices{};
	for (const std::vector<Point*>& row : points)
	{
		for (const Point* point : row)
		{
			vertices.push_back(point->getPos());
		}
	}
	return vertices;
}

std::vector<unsigned int> BezierSurface::createSurfaceIndices() const
{
	std::size_t pointsU{};
	std::size_t pointsV{};
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			pointsU = 3 * m_patchesU + 1;
			pointsV = 3 * m_patchesV + 1;
			break;

		case BezierSurfaceWrapping::u:
			pointsU = 3 * m_patchesU;
			pointsV = 3 * m_patchesV + 1;
			break;

		case BezierSurfaceWrapping::v:
			pointsU = 3 * m_patchesU + 1;
			pointsV = 3 * m_patchesV;
			break;
	}

	std::vector<unsigned int> indices{};
	for (std::size_t patchV = 0; patchV < m_patchesV; ++patchV)
	{
		for (std::size_t patchU = 0; patchU < m_patchesU; ++patchU)
		{
			for (std::size_t v = 3 * patchV; v < 3 * patchV + 4; ++v)
			{
				for (std::size_t u = 3 * patchU; u < 3 * patchU + 4; ++u)
				{
					indices.push_back(static_cast<unsigned int>(v % pointsV * pointsU +
						u % pointsU));
				}
			}
		}
	}
	return indices;
}

std::vector<unsigned int> BezierSurface::createGridIndices() const
{
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			return createGridIndicesNoWrapping();
			
		case BezierSurfaceWrapping::u:
			return createGridIndicesUWrapping();
			
		case BezierSurfaceWrapping::v:
			return createGridIndicesVWrapping();
	}
	return {};
}

std::vector<std::vector<glm::vec3>> BezierSurface::createBoorPointsNoWrapping(const glm::vec3& pos,
	float sizeU, float sizeV) const
{
	std::size_t boorPointsU = m_patchesU + 3;
	std::size_t boorPointsV = m_patchesV + 3;
	float dU = sizeU / m_patchesU;
	float dV = sizeV / m_patchesV;
	glm::vec3 startPoint = pos + glm::vec3{-sizeU / 2, 0, -sizeV / 2};

	std::vector<std::vector<glm::vec3>> points{};
	points.resize(boorPointsV);
	for (std::size_t v = 0; v < boorPointsV; ++v)
	{
		points[v].resize(boorPointsU);
	}

	for (std::size_t v = 0; v < boorPointsV; ++v)
	{
		points[v][0] = startPoint;
		points[v][1] = startPoint + glm::vec3{dU / 3.0f, 0, 0};
		for (std::size_t u = 2; u < boorPointsU - 2; ++u)
		{
			points[v][u] = startPoint + glm::vec3{(u - 1) * dU, 0, 0};
		}
		points[v][boorPointsU - 2] = startPoint +
			glm::vec3{(m_patchesU - 1.0 / 3.0f) * dU, 0, 0};
		points[v][boorPointsU - 1] = startPoint + glm::vec3{m_patchesU * dU, 0, 0};
	}
	for (std::size_t u = 0; u < boorPointsU; ++u)
	{
		points[1][u] = points[1][u] + glm::vec3{0, 0, dV / 3.0f};
		for (std::size_t v = 2; v < boorPointsV - 2; ++v)
		{
			points[v][u] = points[v][u] + glm::vec3{0, 0, (v - 1) * dV};
		}
		points[boorPointsV - 2][u] =
			points[boorPointsV - 2][u] + glm::vec3{0, 0, (m_patchesV - 1.0 / 3.0f) * dV};
		points[boorPointsV - 1][u] = points[boorPointsV - 1][u] + glm::vec3{0, 0, m_patchesV * dV};
	}
	return points;
}

std::vector<std::vector<glm::vec3>> BezierSurface::createBoorPointsUWrapping(const glm::vec3& pos,
	float sizeU, float sizeV) const
{
	std::size_t boorPointsU = m_patchesU;
	std::size_t boorPointsV = m_patchesV + 3;
	static constexpr float pi = glm::pi<float>();
	float dAlpha = 2 * pi / m_patchesU;
	float dV = sizeV / m_patchesV;
	float sin = std::sin(dAlpha / 2);
	float R = 3 * sizeU / (3 - 2 * sin * sin);
	glm::vec3 startPoint = pos + glm::vec3{0, 0, -sizeV / 2};
	
	std::vector<std::vector<glm::vec3>> points{};
	points.resize(boorPointsV);
	for (std::size_t v = 0; v < boorPointsV; ++v)
	{
		points[v].resize(boorPointsU);
	}

	for (std::size_t v = 0; v < boorPointsV; ++v)
	{
		for (std::size_t u = 0; u < boorPointsU; ++u)
		{
			points[v][u] = startPoint + glm::vec3{R * std::cos(u * dAlpha),
				R * std::sin(u * dAlpha), 0};
		}
	}
	for (std::size_t u = 0; u < boorPointsU; ++u)
	{
		points[1][u] = points[1][u] + glm::vec3{0, 0, dV / 3.0f};
		for (std::size_t v = 2; v < boorPointsV - 2; ++v)
		{
			points[v][u] = points[v][u] + glm::vec3{0, 0, (v - 1) * dV};
		}
		points[boorPointsV - 2][u] =
			points[boorPointsV - 2][u] + glm::vec3{0, 0, (m_patchesV - 1.0 / 3.0f) * dV};
		points[boorPointsV - 1][u] = points[boorPointsV - 1][u] + glm::vec3{0, 0, m_patchesV * dV};
	}
	return points;
}

std::vector<std::vector<glm::vec3>> BezierSurface::createBoorPointsVWrapping(const glm::vec3& pos,
	float sizeU, float sizeV) const
{
	std::size_t boorPointsU = m_patchesU + 3;
	std::size_t boorPointsV = m_patchesV;
	static constexpr float pi = glm::pi<float>();
	float dU = sizeU / m_patchesU;
	float dAlpha = 2 * pi / m_patchesV;
	float sin = std::sin(dAlpha / 2);
	float R = 3 * sizeV / (3 - 2 * sin * sin);
	glm::vec3 startPoint = pos + glm::vec3{-sizeU / 2, 0, 0};
	
	std::vector<std::vector<glm::vec3>> points{};
	points.resize(boorPointsV);
	for (std::size_t v = 0; v < boorPointsV; ++v)
	{
		points[v].resize(boorPointsU);
	}

	for (std::size_t v = 0; v < boorPointsV; ++v)
	{
		points[v][0] = startPoint;
		points[v][1] = startPoint + glm::vec3{dU / 3.0f, 0, 0};
		for (std::size_t u = 2; u < boorPointsU - 2; ++u)
		{
			points[v][u] = startPoint + glm::vec3{(u - 1) * dU, 0, 0};
		}
		points[v][boorPointsU - 2] = startPoint + glm::vec3{(m_patchesU - 1.0 / 3.0f) * dU, 0, 0};
		points[v][boorPointsU - 1] = startPoint + glm::vec3{m_patchesU * dU, 0, 0};
	}
	for (std::size_t u = 0; u < boorPointsU; ++u)
	{
		for (std::size_t v = 0; v < boorPointsV; ++v)
		{
			points[v][u] =
				points[v][u] + glm::vec3{0, R * std::sin(v * dAlpha), R * std::cos(v * dAlpha)};
		}
	}
	return points;
}

std::vector<std::vector<glm::vec3>> BezierSurface::createIntermediatePoints(
	const std::vector<std::vector<glm::vec3>>& boorPoints) const
{
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			return createIntermediatePointsNoWrapping(boorPoints);

		case BezierSurfaceWrapping::u:
			return createIntermediatePointsUWrapping(boorPoints);

		case BezierSurfaceWrapping::v:
			return createIntermediatePointsVWrapping(boorPoints);
	}
	return {};
}

std::vector<std::vector<glm::vec3>> BezierSurface::createIntermediatePointsNoWrapping(
	const std::vector<std::vector<glm::vec3>>& boorPoints) const
{
	std::vector<std::vector<glm::vec3>> intermediatePoints(boorPoints.size());
	for (std::size_t v = 0; v < boorPoints.size(); ++v)
	{
		std::vector<glm::vec3> e(m_patchesU + 1);
		std::vector<glm::vec3> f(m_patchesU);
		std::vector<glm::vec3> g(m_patchesU);

		f[0] = boorPoints[v][1];
		if (m_patchesU > 1)
		{
			g[0] = (boorPoints[v][1] + boorPoints[v][2]) / 2.0f;
			for (std::size_t patchU = 1; patchU < m_patchesU - 1; ++patchU)
			{
				f[patchU] = (2.0f * boorPoints[v][patchU + 1] + boorPoints[v][patchU + 2]) / 3.0f;
				g[patchU] = (boorPoints[v][patchU + 1] + 2.0f * boorPoints[v][patchU + 2]) / 3.0f;
			}
			f[m_patchesU - 1] = (boorPoints[v][m_patchesU] +
				boorPoints[v][m_patchesU + 1]) / 2.0f;
		}
		g[m_patchesU - 1] = boorPoints[v][m_patchesU + 1];

		e[0] = boorPoints[v][0];
		for (std::size_t patchU = 1; patchU < m_patchesU; ++patchU)
		{
			e[patchU] = (g[patchU - 1] + f[patchU]) / 2.0f;
		}
		e[m_patchesU] = boorPoints[v][m_patchesU + 2];

		for (std::size_t patchU = 0; patchU < m_patchesU; ++patchU)
		{
			intermediatePoints[v].push_back(e[patchU]);
			intermediatePoints[v].push_back(f[patchU]);
			intermediatePoints[v].push_back(g[patchU]);
		}
		intermediatePoints[v].push_back(e[m_patchesU]);
	}
	return intermediatePoints;
}

std::vector<std::vector<glm::vec3>> BezierSurface::createIntermediatePointsUWrapping(
	const std::vector<std::vector<glm::vec3>>& boorPoints) const
{
	std::vector<std::vector<glm::vec3>> intermediatePoints(boorPoints.size());
	for (std::size_t v = 0; v < boorPoints.size(); ++v)
	{
		std::vector<glm::vec3> e(m_patchesU);
		std::vector<glm::vec3> f(m_patchesU);
		std::vector<glm::vec3> g(m_patchesU);
		
		f[0] = (2.0f * boorPoints[v][0] + boorPoints[v][1]) / 3.0f;
		g[0] = (boorPoints[v][0] + 2.0f * boorPoints[v][1]) / 3.0f;
		for (std::size_t patchU = 1; patchU < m_patchesU; ++patchU)
		{
			f[patchU] =
				(2.0f * boorPoints[v][patchU] + boorPoints[v][(patchU + 1) % m_patchesU]) / 3.0f;
			g[patchU] =
				(boorPoints[v][patchU] + 2.0f * boorPoints[v][(patchU + 1) % m_patchesU]) / 3.0f;
			e[patchU] = (g[patchU - 1] + f[patchU]) / 2.0f;
		}
		e[0] = (g[m_patchesU - 1] + f[0]) / 2.0f;
		
		for (std::size_t patchU = 0; patchU < m_patchesU; ++patchU)
		{
			intermediatePoints[v].push_back(e[patchU]);
			intermediatePoints[v].push_back(f[patchU]);
			intermediatePoints[v].push_back(g[patchU]);
		}
	}
	return intermediatePoints;
}

std::vector<std::vector<glm::vec3>> BezierSurface::createIntermediatePointsVWrapping(
	const std::vector<std::vector<glm::vec3>>& boorPoints) const
{
	return createIntermediatePointsNoWrapping(boorPoints);
}

void BezierSurface::createBezierPointsNoWrapping(
	const std::vector<std::vector<glm::vec3>>& boorPoints,
	std::vector<std::vector<glm::vec3>>& bezierPoints) const
{
	std::vector<std::vector<glm::vec3>> intermediatePoints = createIntermediatePoints(boorPoints);
	for (std::size_t u = 0; u < intermediatePoints[0].size(); ++u)
	{
		std::vector<glm::vec3> e(m_patchesV + 1);
		std::vector<glm::vec3> f(m_patchesV);
		std::vector<glm::vec3> g(m_patchesV);

		f[0] = intermediatePoints[1][u];
		if (m_patchesV > 1)
		{
			g[0] = (intermediatePoints[1][u] + intermediatePoints[2][u]) / 2.0f;
			for (std::size_t patchV = 1; patchV < m_patchesV - 1; ++patchV)
			{
				f[patchV] = (2.0f * intermediatePoints[patchV + 1][u] +
					intermediatePoints[patchV + 2][u]) / 3.0f;
				g[patchV] = (intermediatePoints[patchV + 1][u] +
					2.0f * intermediatePoints[patchV + 2][u]) / 3.0f;
			}
			f[m_patchesV - 1] = (intermediatePoints[m_patchesV][u] +
				intermediatePoints[m_patchesV + 1][u]) / 2.0f;
		}
		g[m_patchesV - 1] = intermediatePoints[m_patchesV + 1][u];

		e[0] = intermediatePoints[0][u];
		for (std::size_t patchV = 1; patchV < m_patchesV; ++patchV)
		{
			e[patchV] = (g[patchV - 1] + f[patchV]) / 2.0f;
		}
		e[m_patchesV] = intermediatePoints[m_patchesV + 2][u];

		for (std::size_t patchV = 0; patchV < m_patchesV; ++patchV)
		{
			bezierPoints[3 * patchV][u] = e[patchV];
			bezierPoints[3 * patchV + 1][u] = f[patchV];
			bezierPoints[3 * patchV + 2][u] = g[patchV];
		}
		bezierPoints[3 * m_patchesV][u] = e[m_patchesV];
	}
}

void BezierSurface::createBezierPointsUWrapping(
	const std::vector<std::vector<glm::vec3>>& boorPoints,
	std::vector<std::vector<glm::vec3>>& bezierPoints) const
{
	return createBezierPointsNoWrapping(boorPoints, bezierPoints);
}

void BezierSurface::createBezierPointsVWrapping(
	const std::vector<std::vector<glm::vec3>>& boorPoints,
	std::vector<std::vector<glm::vec3>>& bezierPoints) const
{
	std::vector<std::vector<glm::vec3>> intermediatePoints = createIntermediatePoints(boorPoints);
	for (std::size_t u = 0; u < intermediatePoints[0].size(); ++u)
	{
		std::vector<glm::vec3> e(m_patchesV);
		std::vector<glm::vec3> f(m_patchesV);
		std::vector<glm::vec3> g(m_patchesV);
		
		f[0] = (2.0f * intermediatePoints[0][u] + intermediatePoints[1][u]) / 3.0f;
		g[0] = (intermediatePoints[0][u] + 2.0f * intermediatePoints[1][u]) / 3.0f;
		for (std::size_t patchV = 1; patchV < m_patchesV; ++patchV)
		{
			f[patchV] = (2.0f * intermediatePoints[patchV][u] +
				intermediatePoints[(patchV + 1) % m_patchesV][u]) / 3.0f;
			g[patchV] = (intermediatePoints[patchV][u] +
				2.0f * intermediatePoints[(patchV + 1) % m_patchesV][u]) / 3.0f;
			e[patchV] = (g[patchV - 1] + f[patchV]) / 2.0f;
		}
		e[0] = (g[m_patchesV - 1] + f[0]) / 2.0f;
		
		for (std::size_t patchV = 0; patchV < m_patchesV; ++patchV)
		{
			bezierPoints[3 * patchV][u] = e[patchV];
			bezierPoints[3 * patchV + 1][u] = f[patchV];
			bezierPoints[3 * patchV + 2][u] = g[patchV];
		}
	}
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
			[this] (void*)
			{
				pointMoveNotification();
			}
		));

	m_pointRereferenceNotifications.push_back(point->registerForRereferenceNotification
		(
			[this] (void* notification, Point* newPoint)
			{
				pointRereferenceNotification(static_cast<Point::RereferenceCallback*>(notification),
					newPoint);
			}
		));

	m_pointDeletabilityLocks.push_back(point->getDeletabilityLock());
}

void BezierSurface::pointMoveNotification()
{
	updateGeometry();
}

void BezierSurface::pointRereferenceNotification(Point::RereferenceCallback* notification,
	Point* newPoint)
{
	auto iterator = std::find_if
	(
		m_pointRereferenceNotifications.begin(), m_pointRereferenceNotifications.end(),
		[notification] (const std::shared_ptr<Point::RereferenceCallback>& sharedNotification)
		{
			return sharedNotification.get() == notification;
		}
	);
	std::size_t notificationIndex = iterator - m_pointRereferenceNotifications.begin();
	std::size_t rowIndex = notificationIndex / m_points[0].size();
	std::size_t pointIndex = notificationIndex % m_points[0].size();

	m_points[rowIndex][pointIndex] = newPoint;

	m_pointMoveNotifications[notificationIndex] = newPoint->registerForMoveNotification
		(
			[this] (void*)
			{
				pointMoveNotification();
			}
		);

	m_pointRereferenceNotifications[notificationIndex] =
		newPoint->registerForRereferenceNotification
		(
			[this] (void* notification, Point* newPoint)
			{
				pointRereferenceNotification(static_cast<Point::RereferenceCallback*>(notification),
					newPoint);
			}
		);

	m_pointDeletabilityLocks[notificationIndex] = newPoint->getDeletabilityLock();

	updateGeometry();
}

std::vector<unsigned int> BezierSurface::createGridIndicesNoWrapping() const
{
	std::vector<unsigned int> indices{};
	for (std::size_t v = 0; v < m_pointsV; ++v)
	{
		for (std::size_t u = 0; u < m_pointsU - 1; ++u)
		{
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u));
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u + 1));
		}
	}
	for (std::size_t u = 0; u < m_pointsU; ++u)
	{
		for (std::size_t v = 0; v < m_pointsV - 1; ++v)
		{
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u));
			indices.push_back(static_cast<unsigned int>((v + 1) * m_pointsU + u));
		}
	}
	return indices;
}

std::vector<unsigned int> BezierSurface::createGridIndicesUWrapping() const
{
	std::vector<unsigned int> indices{};
	for (std::size_t v = 0; v < m_pointsV; ++v)
	{
		for (std::size_t u = 0; u < m_pointsU; ++u)
		{
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u));
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + (u + 1) % m_pointsU));
		}
	}
	for (std::size_t u = 0; u < m_pointsU; ++u)
	{
		for (std::size_t v = 0; v < m_pointsV - 1; ++v)
		{
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u));
			indices.push_back(static_cast<unsigned int>((v + 1) * m_pointsU + u));
		}
	}
	return indices;
}

std::vector<unsigned int> BezierSurface::createGridIndicesVWrapping() const
{
	std::vector<unsigned int> indices{};
	for (std::size_t v = 0; v < m_pointsV; ++v)
	{
		for (std::size_t u = 0; u < m_pointsU - 1; ++u)
		{
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u));
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u + 1));
		}
	}
	for (std::size_t u = 0; u < m_pointsU; ++u)
	{
		for (std::size_t v = 0; v < m_pointsV; ++v)
		{
			indices.push_back(static_cast<unsigned int>(v * m_pointsU + u));
			indices.push_back(static_cast<unsigned int>((v + 1) % m_pointsV * m_pointsU + u));
		}
	}
	return indices;
}
