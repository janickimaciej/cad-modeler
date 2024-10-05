#include "scene.hpp"

#include <algorithm>
#include <iterator>
#include <utility>

static constexpr float viewHeight = 10.0f;
static constexpr float fovYDeg = 60.0f;
static constexpr float nearPlane = 0.1f;
static constexpr float farPlane = 1000.0f;

Scene::Scene(const glm::ivec2& windowSize) :
	m_windowSize{windowSize},
	m_perspectiveCamera{fovYDeg, static_cast<float>(windowSize.x) / windowSize.y, nearPlane,
		farPlane, m_shaderPrograms},
	m_orthographicCamera{viewHeight, static_cast<float>(windowSize.x) / windowSize.y, nearPlane,
		farPlane, m_shaderPrograms},
	m_leftEyeFramebuffer{windowSize}
{
	auto firstModelIter = m_models.begin();

	setCameraType(m_cameraType);
	addPitchCamera(glm::radians(-30.0f));
	addYawCamera(glm::radians(15.0f));

	setUpFramebuffer();
	m_leftEyeFramebuffer.bind();
	setUpFramebuffer();
	m_leftEyeFramebuffer.unbind();
}

void Scene::update()
{
	deleteEmptyBezierCurves();
	deleteInvalidBezierPatches();
	deleteInvalidGregorySurfaces();
	deleteUnreferencedNonDeletablePoints();
}

void Scene::render() const
{
	if (m_anaglyphOn)
	{
		m_leftEyeFramebuffer.bind();
		clearFramebuffer(AnaglyphMode::leftEye);

		m_activeCamera->useLeftEye(m_windowSize);
		renderModels();
		renderCursor();
		renderSelectedModelsCenter();
		renderGrid();

		m_leftEyeFramebuffer.unbind();
		clearFramebuffer(AnaglyphMode::rightEye);

		m_activeCamera->useRightEye(m_windowSize);
		renderModels();
		renderCursor();
		renderSelectedModelsCenter();
		renderGrid();

		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);
		m_leftEyeFramebuffer.bindTexture();
		m_shaderPrograms.quad.use();
		m_leftEyeQuad.render();
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		clearFramebuffer(AnaglyphMode::none);

		m_activeCamera->use(m_windowSize);
		renderModels();
		renderCursor();
		renderSelectedModelsCenter();
		renderGrid();
	}
}

void Scene::updateWindowSize()
{
	setAspectRatio(static_cast<float>(m_windowSize.x) / m_windowSize.y);
	m_leftEyeFramebuffer.resize(m_windowSize);
}

CameraType Scene::getCameraType() const
{
	return m_cameraType;
}

void Scene::setCameraType(CameraType cameraType)
{
	m_cameraType = cameraType;
	switch (cameraType)
	{
		case CameraType::perspective:
			m_activeCamera = &m_perspectiveCamera;
			break;

		case CameraType::orthographic:
			m_activeCamera = &m_orthographicCamera;
			m_anaglyphOn = false;
			break;
	}
}

void Scene::addPitchCamera(float pitchRad)
{
	m_perspectiveCamera.addPitch(pitchRad);
	m_orthographicCamera.addPitch(pitchRad);
}

void Scene::addYawCamera(float yawRad)
{
	m_perspectiveCamera.addYaw(yawRad);
	m_orthographicCamera.addYaw(yawRad);
}

void Scene::moveXCamera(float x)
{
	m_perspectiveCamera.moveX(x);
	m_orthographicCamera.moveX(x);
}

void Scene::moveYCamera(float y)
{
	m_perspectiveCamera.moveY(y);
	m_orthographicCamera.moveY(y);
}

void Scene::zoomCamera(float zoom)
{
	m_perspectiveCamera.zoom(zoom);
	m_orthographicCamera.zoom(zoom);
}

int Scene::getModelCount(ModelType type) const
{
	switch (type)
	{
		case ModelType::all:
			return static_cast<int>(m_models.size());

		case ModelType::point:
			return static_cast<int>(m_points.size());

		case ModelType::torus:
			return static_cast<int>(m_toruses.size());

		case ModelType::bezierCurveC0:
			return static_cast<int>(m_bezierCurvesC0.size());

		case ModelType::bezierCurveC2:
			return static_cast<int>(m_bezierCurvesC2.size());

		case ModelType::bezierCurveInter:
			return static_cast<int>(m_bezierCurvesInter.size());

		case ModelType::bezierPatch:
			return static_cast<int>(m_bezierPatches.size());

		case ModelType::bezierSurfaceC0:
			return static_cast<int>(m_bezierSurfacesC0.size());

		case ModelType::bezierSurfaceC2:
			return static_cast<int>(m_bezierSurfacesC2.size());

		case ModelType::gregorySurface:
			return static_cast<int>(m_gregorySurfaces.size());

		case ModelType::intersectionCurve:
			return static_cast<int>(m_intersectionCurves.size());
	}
	return {};
}

bool Scene::isAnyModelSelected() const
{
	return m_selectedModels.size() > 0;
}

bool Scene::isOneModelSelected() const
{
	return m_selectedModels.size() == 1;
}

bool Scene::isModelVirtual(int i, ModelType type) const
{
	switch (type)
	{
		case ModelType::all:
			return m_models[i]->isVirtual();

		case ModelType::point:
			return m_points[i]->isVirtual();

		case ModelType::torus:
			return m_toruses[i]->isVirtual();

		case ModelType::bezierCurveC0:
			return m_bezierCurvesC0[i]->isVirtual();

		case ModelType::bezierCurveC2:
			return m_bezierCurvesC2[i]->isVirtual();

		case ModelType::bezierCurveInter:
			return m_bezierCurvesInter[i]->isVirtual();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->isVirtual();

		case ModelType::bezierSurfaceC0:
			return m_bezierSurfacesC0[i]->isVirtual();

		case ModelType::bezierSurfaceC2:
			return m_bezierSurfacesC2[i]->isVirtual();

		case ModelType::gregorySurface:
			return m_gregorySurfaces[i]->isVirtual();

		case ModelType::intersectionCurve:
			return m_intersectionCurves[i]->isVirtual();
	}
	return {};
}

bool Scene::isModelSelected(int i, ModelType type) const
{
	switch (type)
	{
		case ModelType::all:
			return m_models[i]->isSelected();

		case ModelType::point:
			return m_points[i]->isSelected();

		case ModelType::torus:
			return m_toruses[i]->isSelected();

		case ModelType::bezierCurveC0:
			return m_bezierCurvesC0[i]->isSelected();

		case ModelType::bezierCurveC2:
			return m_bezierCurvesC2[i]->isSelected();

		case ModelType::bezierCurveInter:
			return m_bezierCurvesInter[i]->isSelected();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->isSelected();

		case ModelType::bezierSurfaceC0:
			return m_bezierSurfacesC0[i]->isSelected();

		case ModelType::bezierSurfaceC2:
			return m_bezierSurfacesC2[i]->isSelected();

		case ModelType::gregorySurface:
			return m_gregorySurfaces[i]->isSelected();

		case ModelType::intersectionCurve:
			return m_intersectionCurves[i]->isSelected();
	}
	return {};
}

void Scene::selectModel(int i, ModelType type)
{
	switch (type)
	{
		case ModelType::all:
			m_models[i]->select();
			m_selectedModels.push_back(m_models[i]);
			break;

		case ModelType::point:
			m_points[i]->select();
			m_selectedModels.push_back(m_points[i].get());
			break;

		case ModelType::torus:
			m_toruses[i]->select();
			m_selectedModels.push_back(m_toruses[i].get());
			break;

		case ModelType::bezierCurveC0:
			m_bezierCurvesC0[i]->select();
			m_selectedModels.push_back(m_bezierCurvesC0[i].get());
			break;

		case ModelType::bezierCurveC2:
			m_bezierCurvesC2[i]->select();
			m_selectedModels.push_back(m_bezierCurvesC2[i].get());
			break;

		case ModelType::bezierCurveInter:
			m_bezierCurvesInter[i]->select();
			m_selectedModels.push_back(m_bezierCurvesInter[i].get());
			break;

		case ModelType::bezierPatch:
			m_bezierPatches[i]->select();
			m_selectedModels.push_back(m_bezierPatches[i].get());
			break;

		case ModelType::bezierSurfaceC0:
			m_bezierSurfacesC0[i]->select();
			m_selectedModels.push_back(m_bezierSurfacesC0[i].get());
			break;

		case ModelType::bezierSurfaceC2:
			m_bezierSurfacesC2[i]->select();
			m_selectedModels.push_back(m_bezierSurfacesC2[i].get());
			break;

		case ModelType::gregorySurface:
			m_gregorySurfaces[i]->select();
			m_selectedModels.push_back(m_gregorySurfaces[i].get());
			break;

		case ModelType::intersectionCurve:
			m_intersectionCurves[i]->select();
			m_selectedModels.push_back(m_intersectionCurves[i].get());
			break;
	}
}

void Scene::deselectModel(int i, ModelType type)
{
	Model* deselectedModel{};
	switch (type)
	{
		case ModelType::all:
			m_models[i]->deselect();
			deselectedModel = m_models[i];
			break;

		case ModelType::point:
			m_points[i]->deselect();
			deselectedModel = m_points[i].get();
			break;

		case ModelType::torus:
			m_toruses[i]->deselect();
			deselectedModel = m_toruses[i].get();
			break;

		case ModelType::bezierCurveC0:
			m_bezierCurvesC0[i]->deselect();
			deselectedModel = m_bezierCurvesC0[i].get();
			break;

		case ModelType::bezierCurveC2:
			m_bezierCurvesC2[i]->deselect();
			deselectedModel = m_bezierCurvesC2[i].get();
			break;

		case ModelType::bezierCurveInter:
			m_bezierCurvesInter[i]->deselect();
			deselectedModel = m_bezierCurvesInter[i].get();
			break;

		case ModelType::bezierPatch:
			m_bezierPatches[i]->deselect();
			deselectedModel = m_bezierPatches[i].get();
			break;

		case ModelType::bezierSurfaceC0:
			m_bezierSurfacesC0[i]->deselect();
			deselectedModel = m_bezierSurfacesC0[i].get();
			break;

		case ModelType::bezierSurfaceC2:
			m_bezierSurfacesC2[i]->deselect();
			deselectedModel = m_bezierSurfacesC2[i].get();
			break;

		case ModelType::gregorySurface:
			m_gregorySurfaces[i]->deselect();
			deselectedModel = m_gregorySurfaces[i].get();
			break;

		case ModelType::intersectionCurve:
			m_intersectionCurves[i]->deselect();
			deselectedModel = m_intersectionCurves[i].get();
			break;
	}
	std::erase_if
	(
		m_selectedModels,
		[deselectedModel] (Model* model)
		{
			return model == deselectedModel;
		}
	);
}

void Scene::toggleModel(int i, ModelType type)
{
	if (isModelSelected(i, type))
	{
		deselectModel(i, type);
	}
	else
	{
		selectModel(i, type);
	}
}

void Scene::deselectAllModels()
{
	for (Model* model : m_selectedModels)
	{
		model->deselect();
	}
	m_selectedModels.clear();
}

void Scene::deleteSelectedModels()
{
	std::erase_if
	(
		m_models,
		[] (Model* model)
		{
			return model->isSelected() && model->isDeletable();
		}
	);
	std::erase_if
	(
		m_selectedModels,
		[] (Model* model)
		{
			return model->isDeletable();
		}
	);

	deleteSelectedModels(m_points);
	deleteSelectedModels(m_toruses);
	deleteSelectedModels(m_bezierCurvesC0);
	deleteSelectedModels(m_bezierCurvesC2);
	deleteSelectedModels(m_bezierCurvesInter);
	deleteSelectedModels(m_bezierPatches);
	deleteSelectedModels(m_bezierSurfacesC0);
	deleteSelectedModels(m_bezierSurfacesC2);
	deleteSelectedModels(m_gregorySurfaces);
	deleteSelectedModels(m_intersectionCurves);
}

bool Scene::selectUniqueModel(const glm::vec2& screenPos)
{
	std::optional<int> closestModel = getClosestModel(screenPos);
	if (!closestModel.has_value())
	{
		return false;
	}

	deselectAllModels();
	selectModel(*closestModel);
	return true;
}

void Scene::toggleModel(const glm::vec2& screenPos)
{
	std::optional<int> closestModel = getClosestModel(screenPos);
	if (!closestModel.has_value())
	{
		return;
	}

	toggleModel(*closestModel);
}

void Scene::moveUniqueSelectedModel(const glm::vec2& screenPos) const
{
	Model* selectedModel = getUniqueSelectedModel();
	if (selectedModel != nullptr)
	{
		selectedModel->setScreenPos(screenPos, m_activeCamera->getMatrix(), m_windowSize);
	}
}

void Scene::collapse2Points()
{
	std::vector<Point*> points = getNonVirtualSelectedPoints();
	if (points.size() != 2)
	{
		return;
	}

	Point* oldPoint = points[0];
	Point* newPoint = points[1];
	if (!oldPoint->isDeletable() && newPoint->isDeletable())
	{
		std::swap(oldPoint, newPoint);
	}

	glm::vec3 newPos = (oldPoint->getPos() + newPoint->getPos()) / 2.0f;
	newPoint->setPos(newPos);

	oldPoint->rereference(newPoint);
	std::erase(m_models, oldPoint);
	std::erase_if
	(
		m_points,
		[oldPoint] (const std::unique_ptr<Point>& point)
		{
			return point.get() == oldPoint;
		}
	);
}

BezierPatch* Scene::getUniqueSelectedBezierPatch() const
{
	if (m_selectedModels.size() != 1)
	{
		return nullptr;
	}

	for (const std::unique_ptr<BezierPatch>& patch : m_bezierPatches)
	{
		if (patch->isSelected())
		{
			return patch.get();
		}
	}

	return nullptr;
}

const Intersectable* Scene::getUniqueSelectedIntersectable() const
{
	if (m_selectedModels.size() != 1)
	{
		return nullptr;
	}

	for (const std::unique_ptr<Torus>& torus : m_toruses)
	{
		if (torus->isSelected())
		{
			return torus.get();
		}
	}

	for (const std::unique_ptr<BezierPatch>& patch : m_bezierPatches)
	{
		if (patch->isSelected())
		{
			return patch.get();
		}
	}

	for (const std::unique_ptr<BezierSurfaceC0>& surface : m_bezierSurfacesC0)
	{
		if (surface->isSelected())
		{
			return surface.get();
		}
	}

	for (const std::unique_ptr<BezierSurfaceC2>& surface : m_bezierSurfacesC2)
	{
		if (surface->isSelected())
		{
			return surface.get();
		}
	}

	return nullptr;
}

void Scene::rotateXSelectedModels(float angleRad)
{
	m_selectedModelsCenter.rotateX(angleRad);
}

void Scene::rotateYSelectedModels(float angleRad)
{
	m_selectedModelsCenter.rotateY(angleRad);
}

void Scene::rotateZSelectedModels(float angleRad)
{
	m_selectedModelsCenter.rotateZ(angleRad);
}

void Scene::scaleXSelectedModels(float scale)
{
	m_selectedModelsCenter.scaleX(scale);
}

void Scene::scaleYSelectedModels(float scale)
{
	m_selectedModelsCenter.scaleY(scale);
}

void Scene::scaleZSelectedModels(float scale)
{
	m_selectedModelsCenter.scaleZ(scale);
}

std::string Scene::getUniqueSelectedModelName() const
{
	Model* selectedModel = getUniqueSelectedModel();
	if (selectedModel != nullptr)
	{
		return selectedModel->getName();
	}
	return "";
}

void Scene::setUniqueSelectedModelName(const std::string& name) const
{
	Model* selectedModel = getUniqueSelectedModel();
	if (selectedModel != nullptr)
	{
		selectedModel->setName(name);
	}
}

std::string Scene::getModelOriginalName(int i, ModelType type) const
{
	switch (type)
	{
		case ModelType::all:
			return m_models[i]->getOriginalName();

		case ModelType::point:
			return m_points[i]->getOriginalName();

		case ModelType::torus:
			return m_toruses[i]->getOriginalName();

		case ModelType::bezierCurveC0:
			return m_bezierCurvesC0[i]->getOriginalName();

		case ModelType::bezierCurveC2:
			return m_bezierCurvesC2[i]->getOriginalName();

		case ModelType::bezierCurveInter:
			return m_bezierCurvesInter[i]->getOriginalName();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->getOriginalName();

		case ModelType::bezierSurfaceC0:
			return m_bezierSurfacesC0[i]->getOriginalName();

		case ModelType::bezierSurfaceC2:
			return m_bezierSurfacesC2[i]->getOriginalName();

		case ModelType::gregorySurface:
			return m_gregorySurfaces[i]->getOriginalName();

		case ModelType::intersectionCurve:
			return m_intersectionCurves[i]->getOriginalName();
	}
	return {};
}

std::string Scene::getModelName(int i, ModelType type) const
{
	switch (type)
	{
		case ModelType::all:
			return m_models[i]->getName();

		case ModelType::point:
			return m_points[i]->getName();

		case ModelType::torus:
			return m_toruses[i]->getName();

		case ModelType::bezierCurveC0:
			return m_bezierCurvesC0[i]->getName();

		case ModelType::bezierCurveC2:
			return m_bezierCurvesC2[i]->getName();

		case ModelType::bezierCurveInter:
			return m_bezierCurvesInter[i]->getName();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->getName();

		case ModelType::bezierSurfaceC0:
			return m_bezierSurfacesC0[i]->getName();

		case ModelType::bezierSurfaceC2:
			return m_bezierSurfacesC2[i]->getName();

		case ModelType::gregorySurface:
			return m_gregorySurfaces[i]->getName();

		case ModelType::intersectionCurve:
			return m_intersectionCurves[i]->getName();
	}
	return {};
}

void Scene::addPoint()
{
	std::unique_ptr<Point> point = std::make_unique<Point>(m_shaderPrograms.point,
		m_cursor.getPos());

	if (m_selectedModels.size() == 1)
	{
		auto selectedBezierCurveC0 = std::find_if
		(
			m_bezierCurvesC0.begin(), m_bezierCurvesC0.end(),
			[] (const std::unique_ptr<BezierCurveC0>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedBezierCurveC0 != m_bezierCurvesC0.end())
		{
			(*selectedBezierCurveC0)->addPoints({point.get()});
		}

		auto selectedBezierCurveC2 = std::find_if
		(
			m_bezierCurvesC2.begin(), m_bezierCurvesC2.end(),
			[] (const std::unique_ptr<BezierCurveC2>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedBezierCurveC2 != m_bezierCurvesC2.end())
		{
			std::vector<std::unique_ptr<Point>> newPoints{};
			(*selectedBezierCurveC2)->addPoints({point.get()}, newPoints);
			addPoints(std::move(newPoints));
		}

		auto selectedBezierCurveInter = std::find_if
		(
			m_bezierCurvesInter.begin(), m_bezierCurvesInter.end(),
			[] (const std::unique_ptr<BezierCurveInter>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedBezierCurveInter != m_bezierCurvesInter.end())
		{
			(*selectedBezierCurveInter)->addPoints({point.get()});
		}
	}

	m_models.push_back(point.get());
	m_points.push_back(std::move(point));
}

void Scene::addTorus()
{
	std::unique_ptr<Torus> torus = std::make_unique<Torus>(m_shaderPrograms.mesh,
		m_cursor.getPos());
	m_models.push_back(torus.get());
	m_toruses.push_back(std::move(torus));
}

void Scene::addBezierCurveC0()
{
	std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
	if (m_selectedModels.size() != nonVirtualSelectedPoints.size() ||
		nonVirtualSelectedPoints.size() == 0)
	{
		return;
	}

	std::unique_ptr<BezierCurveC0> curve = std::make_unique<BezierCurveC0>(
		m_shaderPrograms.bezierCurve, m_shaderPrograms.polyline,
		nonVirtualSelectedPoints, m_bezierCurveSelfDestructCallback);
	m_models.push_back(curve.get());
	m_bezierCurvesC0.push_back(std::move(curve));
}

void Scene::addBezierCurveC2()
{
	std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
	if (m_selectedModels.size() != nonVirtualSelectedPoints.size() ||
		nonVirtualSelectedPoints.size() == 0)
	{
		return;
	}

	std::vector<std::unique_ptr<Point>> newPoints{};
	std::unique_ptr<BezierCurveC2> curve = std::make_unique<BezierCurveC2>(
		m_shaderPrograms.bezierCurve, m_shaderPrograms.polyline, m_shaderPrograms.point,
		nonVirtualSelectedPoints, m_bezierCurveSelfDestructCallback, newPoints);
	m_models.push_back(curve.get());
	m_bezierCurvesC2.push_back(std::move(curve));
	addPoints(std::move(newPoints));
}

void Scene::addBezierCurveInter()
{
	std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
	if (m_selectedModels.size() != nonVirtualSelectedPoints.size() ||
		nonVirtualSelectedPoints.size() == 0)
	{
		return;
	}

	std::unique_ptr<BezierCurveInter> curve = std::make_unique<BezierCurveInter>(
		m_shaderPrograms.bezierCurveInter, m_shaderPrograms.polyline,
		nonVirtualSelectedPoints, m_bezierCurveSelfDestructCallback);
	m_models.push_back(curve.get());
	m_bezierCurvesInter.push_back(std::move(curve));
}

void Scene::addSelectedPointsToCurve()
{
	auto selectedBezierCurveC0 = std::find_if
	(
		m_bezierCurvesC0.begin(), m_bezierCurvesC0.end(),
		[] (const std::unique_ptr<BezierCurveC0>& curve)
		{
			return curve->isSelected();
		}
	);
	if (selectedBezierCurveC0 != m_bezierCurvesC0.end())
	{
		std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
		if (m_selectedModels.size() == nonVirtualSelectedPoints.size() + 1 &&
			nonVirtualSelectedPoints.size() != 0)
		{
			(*selectedBezierCurveC0)->addPoints(nonVirtualSelectedPoints);
		}
	}

	auto selectedBezierCurveC2 = std::find_if
	(
		m_bezierCurvesC2.begin(), m_bezierCurvesC2.end(),
		[] (const std::unique_ptr<BezierCurveC2>& curve)
		{
			return curve->isSelected();
		}
	);
	if (selectedBezierCurveC2 != m_bezierCurvesC2.end())
	{
		std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
		if (m_selectedModels.size() == nonVirtualSelectedPoints.size() + 1 &&
			nonVirtualSelectedPoints.size() != 0)
		{
			std::vector<std::unique_ptr<Point>> newPoints{};
			(*selectedBezierCurveC2)->addPoints(nonVirtualSelectedPoints, newPoints);
			addPoints(std::move(newPoints));
		}
	}

	auto selectedBezierCurveInter = std::find_if
	(
		m_bezierCurvesInter.begin(), m_bezierCurvesInter.end(),
		[] (const std::unique_ptr<BezierCurveInter>& curve)
		{
			return curve->isSelected();
		}
	);
	if (selectedBezierCurveInter != m_bezierCurvesInter.end())
	{
		std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
		if (m_selectedModels.size() == nonVirtualSelectedPoints.size() + 1 &&
			nonVirtualSelectedPoints.size() != 0)
		{
			(*selectedBezierCurveInter)->addPoints(nonVirtualSelectedPoints);
		}
	}
}

void Scene::addBezierSurfaceC0(int patchesU, int patchesV, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping)
{
	std::vector<std::unique_ptr<Point>> newPoints{};
	std::vector<std::unique_ptr<BezierPatch>> newPatches{};
	std::unique_ptr<BezierSurfaceC0> surface = std::make_unique<BezierSurfaceC0>(
		m_shaderPrograms.bezierSurface, m_shaderPrograms.mesh, m_shaderPrograms.point, patchesU,
		patchesV, m_cursor.getPos(), sizeU, sizeV, wrapping, newPoints, newPatches);
	addPoints(std::move(newPoints));
	addBezierPatches(std::move(newPatches));
	m_models.push_back(surface.get());
	m_bezierSurfacesC0.push_back(std::move(surface));
}

void Scene::addBezierSurfaceC2(int patchesU, int patchesV, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping)
{
	std::vector<std::unique_ptr<Point>> newPoints{};
	std::vector<std::unique_ptr<BezierPatch>> newPatches{};
	std::unique_ptr<BezierSurfaceC2> surface = std::make_unique<BezierSurfaceC2>(
		m_shaderPrograms.bezierSurface, m_shaderPrograms.mesh, m_shaderPrograms.point, patchesU,
		patchesV, m_cursor.getPos(), sizeU, sizeV, wrapping, newPoints, newPatches);
	addPoints(std::move(newPoints));
	addBezierPatches(std::move(newPatches));
	m_models.push_back(surface.get());
	m_bezierSurfacesC2.push_back(std::move(surface));
}

void Scene::addGregorySurface(const std::array<BezierPatch*, 3>& patches)
{
	std::unique_ptr<GregorySurface> surface = GregorySurface::create(
		m_shaderPrograms.gregorySurface, m_shaderPrograms.vectors, patches,
		m_gregorySurfaceSelfDestructCallback);

	if (surface != nullptr)
	{
		m_models.push_back(surface.get());
		m_gregorySurfaces.push_back(std::move(surface));
	}
}

void Scene::addIntersection(const std::array<const Intersectable*, 2>& surfaces, bool useCursor)
{
	std::unique_ptr<IntersectionCurve> intersectionCurve{};
	if (useCursor)
	{
		intersectionCurve = IntersectionCurve::create(m_shaderPrograms.polyline, surfaces,
			m_cursor.getPos());
	}
	else
	{
		intersectionCurve = IntersectionCurve::create(m_shaderPrograms.polyline, surfaces);
	}

	if (intersectionCurve != nullptr)
	{
		m_models.push_back(intersectionCurve.get());
		m_intersectionCurves.push_back(std::move(intersectionCurve));
	}
}

void Scene::updateActiveCameraGUI()
{
	m_activeCamera->updateGUI();
}

void Scene::updateCursorGUI()
{
	m_cursor.updateGUI(m_activeCamera->getMatrix(), m_windowSize);
}

void Scene::updateSelectedModelsCenterGUI()
{
	m_selectedModelsCenter.updateGUI();
}

void Scene::updateModelGUI(int i, ModelType type)
{
	switch (type)
	{
		case ModelType::all:
			m_models[i]->updateGUI();
			break;

		case ModelType::point:
			m_points[i]->updateGUI();
			break;

		case ModelType::torus:
			m_toruses[i]->updateGUI();
			break;

		case ModelType::bezierCurveC0:
			m_bezierCurvesC0[i]->updateGUI();
			break;

		case ModelType::bezierCurveC2:
			m_bezierCurvesC2[i]->updateGUI();
			break;

		case ModelType::bezierCurveInter:
			m_bezierCurvesInter[i]->updateGUI();
			break;

		case ModelType::bezierPatch:
			m_bezierPatches[i]->updateGUI();
			break;

		case ModelType::bezierSurfaceC0:
			m_bezierSurfacesC0[i]->updateGUI();
			break;

		case ModelType::bezierSurfaceC2:
			m_bezierSurfacesC2[i]->updateGUI();
			break;

		case ModelType::gregorySurface:
			m_gregorySurfaces[i]->updateGUI();
			break;

		case ModelType::intersectionCurve:
			m_intersectionCurves[i]->updateGUI();
			break;
	}
}

bool Scene::getAnaglyphOn() const
{
	return m_anaglyphOn;
}

void Scene::setAnaglyphOn(bool anaglyphOn)
{
	m_anaglyphOn = anaglyphOn;
}

float Scene::getEyesDistance() const
{
	return m_activeCamera->getEyesDistance();
}

void Scene::setEyesDistance(float eyesDistance)
{
	m_perspectiveCamera.setEyesDistance(eyesDistance);
	m_orthographicCamera.setEyesDistance(eyesDistance);
}

float Scene::getScreenDistance() const
{
	return m_activeCamera->getScreenDistance();
}

void Scene::setScreenDistance(float screenDistance)
{
	m_perspectiveCamera.setScreenDistance(screenDistance);
	m_orthographicCamera.setScreenDistance(screenDistance);
}

float Scene::getProjectionPlane() const
{
	return m_activeCamera->getProjectionPlane();
}

void Scene::setProjectionPlane(float projectionPlane)
{
	m_perspectiveCamera.setProjectionPlane(projectionPlane);
	m_orthographicCamera.setProjectionPlane(projectionPlane);
}

void Scene::setUpFramebuffer() const
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
}

void Scene::clearFramebuffer(AnaglyphMode anaglyphMode) const
{
	static constexpr glm::vec3 backgroundColor{0.1f, 0.1f, 0.1f};
	switch (anaglyphMode)
	{
		case AnaglyphMode::none:
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
		break;

		case AnaglyphMode::leftEye:
		glClearColor(backgroundColor.r, 0, 0, 1.0f);
		break;

		case AnaglyphMode::rightEye:
		glClearColor(0, backgroundColor.g, backgroundColor.b, 1.0f);
		break;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::setAspectRatio(float aspectRatio)
{
	m_perspectiveCamera.setAspectRatio(aspectRatio);
	m_orthographicCamera.setAspectRatio(aspectRatio);
}

void Scene::renderModels() const
{
	for (const Model* model : m_models)
	{
		model->render();
	}
}

void Scene::renderCursor() const
{
	m_shaderPrograms.cursor.use();
	m_cursor.render();
}

void Scene::renderSelectedModelsCenter() const
{
	m_shaderPrograms.cursor.use();
	m_selectedModelsCenter.render();
}

void Scene::renderGrid() const
{
	m_shaderPrograms.plane.use();
	m_plane.render(m_cameraType);
}

Model* Scene::getUniqueSelectedModel() const
{
	if (m_selectedModels.size() == 1)
	{
		return m_selectedModels[0];
	}
	return nullptr;
}

std::optional<int> Scene::getClosestModel(const glm::vec2& screenPos) const
{
	std::optional<int> index = std::nullopt;
	static constexpr float treshold = 30;
	float minScreenDistanceSquared = treshold * treshold;
	glm::mat4 cameraMatrix = m_activeCamera->getMatrix();
	for (int i = 0; i < m_models.size(); ++i)
	{
		glm::vec2 modelScreenPos = m_models[i]->getScreenPos(cameraMatrix, m_windowSize);
		glm::vec2 relativePos = modelScreenPos - screenPos;
		float screenDistanceSquared = glm::dot(relativePos, relativePos);
		if (screenDistanceSquared < minScreenDistanceSquared)
		{
			index = i;
			minScreenDistanceSquared = screenDistanceSquared;
		}
	}

	return index;
}

std::vector<Point*> Scene::getNonVirtualSelectedPoints() const
{
	std::vector<Point*> selectedPoints{};
	for (const std::unique_ptr<Point>& point : m_points)
	{
		if (!point->isVirtual() && point->isSelected())
		{
			selectedPoints.push_back(point.get());
		}
	}
	return selectedPoints;
}

void Scene::addPoints(std::vector<std::unique_ptr<Point>> points)
{
	for (const std::unique_ptr<Point>& point : points)
	{
		m_models.push_back(point.get());
	}
	m_points.insert(m_points.end(), std::make_move_iterator(points.begin()),
		std::make_move_iterator(points.end()));
}

void Scene::addBezierPatches(std::vector<std::unique_ptr<BezierPatch>> patches)
{
	for (const std::unique_ptr<BezierPatch>& patch : patches)
	{
		m_models.push_back(patch.get());
	}
	m_bezierPatches.insert(m_bezierPatches.end(), std::make_move_iterator(patches.begin()),
		std::make_move_iterator(patches.end()));
}

void Scene::addBezierCurveForDeletion(const BezierCurve* curve)
{
	m_bezierCurvesToBeDeleted.push_back(curve);
}

void Scene::addGregorySurfaceForDeletion(const GregorySurface* surface)
{
	m_gregorySurfacesToBeDeleted.push_back(surface);
}

void Scene::deleteEmptyBezierCurves()
{
	for (const BezierCurve* curve : m_bezierCurvesToBeDeleted)
	{
		std::erase(m_models, curve);
		std::erase(m_selectedModels, curve);

		std::erase_if
		(
			m_bezierCurvesC0,
			[curve] (const std::unique_ptr<BezierCurveC0>& curveC0)
			{
				return curveC0.get() == curve;
			}
		);
		std::erase_if
		(
			m_bezierCurvesC2,
			[curve] (const std::unique_ptr<BezierCurveC2>& curveC2)
			{
				return curveC2.get() == curve;
			}
		);
		std::erase_if
		(
			m_bezierCurvesInter,
			[curve] (const std::unique_ptr<BezierCurveInter>& curveInter)
			{
				return curveInter.get() == curve;
			}
		);
	}
	m_bezierCurvesToBeDeleted.clear();
}

void Scene::deleteInvalidBezierPatches()
{
	std::vector<BezierPatch*> patchesToBeDeleted{};
	std::erase_if
	(
		m_bezierPatches,
		[&patchesToBeDeleted] (const std::unique_ptr<BezierPatch>& patch)
		{
			if (patch->isInvalid())
			{
				patchesToBeDeleted.push_back(patch.get());
				return true;
			}
			return false;
		}
	);
	for (const BezierPatch* patch : patchesToBeDeleted)
	{
		std::erase(m_models, patch);
	}
}

void Scene::deleteInvalidGregorySurfaces()
{
	for (const GregorySurface* surface : m_gregorySurfacesToBeDeleted)
	{
		std::erase(m_models, surface);
		std::erase(m_selectedModels, surface);

		std::erase_if
		(
			m_gregorySurfaces,
			[surface] (const std::unique_ptr<GregorySurface>& gregorySurface)
			{
				return gregorySurface.get() == surface;
			}
		);
	}
	m_gregorySurfacesToBeDeleted.clear();
}

void Scene::deleteUnreferencedNonDeletablePoints()
{
	std::vector<Point*> pointsToBeDeleted{};
	std::erase_if(m_points,
		[&pointsToBeDeleted] (const std::unique_ptr<Point>& point)
		{
			if (!point->isDeletable() && !point->isReferenced())
			{
				pointsToBeDeleted.push_back(point.get());
				return true;
			}
			return false;
		}
	);
	for (const Point* point : pointsToBeDeleted)
	{
		std::erase(m_models, point);
	}
}
