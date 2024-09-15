#include "models/bezierSurfaces/bezierSurface.hpp"

#include <glm/gtc/constants.hpp>

#include <string>

BezierSurface::BezierSurface(const std::string& name,
	const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV,
	BezierSurfaceWrapping wrapping) :
	Model{{}, name},
	m_gridShaderProgram{bezierSurfaceGridShaderProgram},
	m_patchesU{static_cast<std::size_t>(patchesU)},
	m_patchesV{static_cast<std::size_t>(patchesV)},
	m_wrapping{wrapping}
{ }

BezierSurface::~BezierSurface()
{
	notifyDestroy();
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

	for (const std::vector<BezierPatch*>& row : m_patches)
	{
		for (BezierPatch* patch : row)
		{
			patch->setInvalid();
		}
	}
}

void BezierSurface::render() const
{
	updateShaders();
	if (getRenderGrid())
	{
		m_gridShaderProgram.use();
		m_gridMesh->render();
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
	m_pos = pos / static_cast<float>(m_points[0].size() * m_points.size());
}

void BezierSurface::updatePatches()
{
	for (std::size_t patchV = 0; patchV < m_patchesV; ++patchV)
	{
		for (std::size_t patchU = 0; patchU < m_patchesU; ++patchU)
		{
			m_patches[patchV][patchU]->updatePoints(getBezierPoints(patchU, patchV));
		}
	}
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
	std::size_t bezierPointsU = getBezierPointsU();
	std::size_t bezierPointsV = getBezierPointsV();

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
	glm::vec3 startPoint = pos + glm::vec3{-sizeU / 2 - dU, 0, -sizeV / 2 - dV};

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
			points[v][u] = startPoint + glm::vec3{u * dU, 0, v * dV};
		}
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
	glm::vec3 startPoint = pos + glm::vec3{0, 0, -sizeV / 2 - dV};
	
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
		for (std::size_t v = 0; v < boorPointsV; ++v)
		{
			points[v][u] = points[v][u] + glm::vec3{0, 0, v * dV};
		}
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
	glm::vec3 startPoint = pos + glm::vec3{-sizeU / 2 - dU, 0, 0};
	
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
			points[v][u] = startPoint + glm::vec3{u * dU, 0, 0};
		}
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
		
		glm::vec3 gFirst = (boorPoints[v][0] + 2.0f * boorPoints[v][1]) / 3.0f;
		for (std::size_t patchU = 0; patchU < m_patchesU; ++patchU)
		{
			f[patchU] = (2.0f * boorPoints[v][patchU + 1] + boorPoints[v][patchU + 2]) / 3.0f;
			g[patchU] = (boorPoints[v][patchU + 1] + 2.0f * boorPoints[v][patchU + 2]) / 3.0f;
		}
		glm::vec3 fLast = (2.0f * boorPoints[v][boorPoints[v].size() - 2] +
			boorPoints[v][boorPoints[v].size() - 1]) / 3.0f;

		e[0] = (gFirst + f[0]) / 2.0f;
		for (std::size_t patchU = 1; patchU < m_patchesU; ++patchU)
		{
			e[patchU] = (g[patchU - 1] + f[patchU]) / 2.0f;
		}
		e[m_patchesU] = (g[m_patchesU - 1] + fLast) / 2.0f;

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
		
		glm::vec3 gFirst = (intermediatePoints[0][u] + 2.0f * intermediatePoints[1][u]) / 3.0f;
		for (std::size_t patchV = 0; patchV < m_patchesV; ++patchV)
		{
			f[patchV] = (2.0f * intermediatePoints[patchV + 1][u] +
				intermediatePoints[patchV + 2][u]) / 3.0f;
			g[patchV] = (intermediatePoints[patchV + 1][u] +
				2.0f * intermediatePoints[patchV + 2][u]) / 3.0f;
		}
		glm::vec3 fLast = (2.0f * intermediatePoints[intermediatePoints.size() - 2][u] +
			intermediatePoints[intermediatePoints.size() - 1][u]) / 3.0f;
		
		e[0] = (gFirst + f[0]) / 2.0f;
		for (std::size_t patchV = 1; patchV < m_patchesV; ++patchV)
		{
			e[patchV] = (g[patchV - 1] + f[patchV]) / 2.0f;
		}
		e[m_patchesV] = (g[m_patchesV - 1] + fLast) / 2.0f;

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

std::shared_ptr<BezierSurface::DestroyCallback> BezierSurface::registerForDestroyNotification(
	const DestroyCallback& callback)
{
	std::shared_ptr<DestroyCallback> notification = std::make_shared<DestroyCallback>(callback);
	m_destroyNotifications.push_back(notification);
	return notification;
}

glm::vec3 BezierSurface::surface(float u, float v) const
{
	int patchU{};
	int patchV{};
	float localU{};
	float localV{};
	mapToPatch(u, v, patchU, patchV, localU, localV);

	return m_patches[patchV][patchU]->surface(localU, localV);
}

glm::vec3 BezierSurface::surfaceDU(float u, float v) const
{
	int patchU{};
	int patchV{};
	float localU{};
	float localV{};
	mapToPatch(u, v, patchU, patchV, localU, localV);

	return static_cast<float>(m_patchesU) * m_patches[patchV][patchU]->surfaceDU(localU, localV);
}

glm::vec3 BezierSurface::surfaceDV(float u, float v) const
{
	int patchU{};
	int patchV{};
	float localU{};
	float localV{};
	mapToPatch(u, v, patchU, patchV, localU, localV);

	return static_cast<float>(m_patchesV) * m_patches[patchV][patchU]->surfaceDV(localU, localV);
}

void BezierSurface::mapToPatch(float u, float v, int& patchU, int& patchV, float& localU,
	float& localV) const
{
	if (m_wrapping == BezierSurfaceWrapping::u && (u < 0 || u >= 1))
	{
		u -= std::floor(u);
	}
	if (m_wrapping == BezierSurfaceWrapping::v && (v < 0 || v >= 1))
	{
		v -= std::floor(v);
	}

	float uScaled = m_patchesU * u;
	float vScaled = m_patchesV * v;

	patchU = static_cast<int>(uScaled);
	patchV = static_cast<int>(vScaled);
	if (u < 0)
	{
		patchU = 0;
	}
	if (u >= 1)
	{
		patchU = static_cast<int>(m_patchesU) - 1;
	}
	if (v < 0)
	{
		patchV = 0;
	}
	if (v >= 1)
	{
		patchV = static_cast<int>(m_patchesV) - 1;
	}

	localU = uScaled - patchU;
	localV = vScaled - patchV;
}

bool BezierSurface::uWrapped() const
{
	return m_wrapping == BezierSurfaceWrapping::u;
}

bool BezierSurface::vWrapped() const
{
	return m_wrapping == BezierSurfaceWrapping::v;
}

std::size_t BezierSurface::getBezierPointsU() const
{
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			return 3 * m_patchesU + 1;

		case BezierSurfaceWrapping::u:
			return 3 * m_patchesU;

		case BezierSurfaceWrapping::v:
			return 3 * m_patchesU + 1;
	}
	return{};
}

std::size_t BezierSurface::getBezierPointsV() const
{
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			return 3 * m_patchesV + 1;

		case BezierSurfaceWrapping::u:
			return 3 * m_patchesV + 1;

		case BezierSurfaceWrapping::v:
			return 3 * m_patchesV;
	}
	return{};
}

std::vector<std::unique_ptr<BezierPatch>> BezierSurface::createPatches(
	const ShaderProgram& bezierSurfaceShaderProgram)
{
	std::vector<std::unique_ptr<BezierPatch>> patches{};
	m_patches.resize(m_patchesV);
	for (std::size_t patchV = 0; patchV < m_patchesV; ++patchV)
	{
		for (std::size_t patchU = 0; patchU < m_patchesU; ++patchU)
		{
			bool isOnNegativeUEdge = patchU == 0;
			bool isOnPositiveUEdge = patchU == m_patchesU - 1;
			bool isOnNegativeVEdge = patchV == 0;
			bool isOnPositiveVEdge = patchV == m_patchesV - 1;
			patches.push_back(std::make_unique<BezierPatch>(bezierSurfaceShaderProgram,
				getBezierPoints(patchU, patchV), *this, isOnNegativeUEdge, isOnPositiveUEdge,
				isOnNegativeVEdge, isOnPositiveVEdge));
			m_patches[patchV].push_back(patches.back().get());
		}
	}
	return patches;
}

void BezierSurface::updateGeometry()
{
	updatePos();
	updatePatches();
	updateGridMesh();
}

void BezierSurface::updateShaders() const
{
	if (m_renderGrid)
	{
		m_gridShaderProgram.use();
		m_gridShaderProgram.setUniform("modelMatrix", m_modelMatrix);
		m_gridShaderProgram.setUniform("isDark", true);
		m_gridShaderProgram.setUniform("isSelected", isSelected());
	}
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

void BezierSurface::notifyDestroy()
{
	clearExpiredNotifications();

	for (const std::weak_ptr<DestroyCallback>& notification : m_destroyNotifications)
	{
		std::shared_ptr<DestroyCallback> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)();
		}
	}
}

void BezierSurface::clearExpiredNotifications()
{
	std::erase_if
	(
		m_destroyNotifications,
		[] (const std::weak_ptr<DestroyCallback>& notification)
		{
			return notification.expired();
		}
	);
}
