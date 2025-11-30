#include "scene.hpp"

#include "guis/leftPanel.hpp"
#include "shaderPrograms.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <fstream>
#include <iterator>
#include <utility>
#include <iostream>

static constexpr float viewHeight = 10.0f;
static constexpr float fovYDeg = 60.0f;
static constexpr float nearPlane = 0.1f;
static constexpr float farPlane = 1000.0f;

static inline constexpr float baseHeight = 2.5f;
static inline constexpr float yDefault = 6.6f;

static inline constexpr float path1Radius = 0.8f;
static inline constexpr float path1SegmentingOffset = 0.02f;
static inline constexpr float path1InaccuracyOffset = 0.02f;
static inline constexpr float path1Offset =
	path1Radius + path1SegmentingOffset + path1InaccuracyOffset;
static inline constexpr float path1IntermediateLevel = 1.25f;

static inline constexpr float path2Radius = 0.5f;

Scene::Scene(const glm::ivec2& windowSize) :
	m_perspectiveCamera{windowSize, fovYDeg, nearPlane, farPlane},
	m_orthographicCamera{windowSize, viewHeight, nearPlane, farPlane},
	m_leftEyeFramebuffer{windowSize},
	m_heightmapCamera{heightmapSize, 15, nearPlane, farPlane}
{
	auto firstModelIter = m_models.begin();

	setCameraType(m_cameraType);
	addPitchCamera(glm::radians(-30.0f));
	addYawCamera(glm::radians(15.0f));

	setUpFramebuffer();
	m_leftEyeFramebuffer.bind();
	setUpFramebuffer();
	m_leftEyeFramebuffer.unbind();

	m_heightmapCamera.addPitch(glm::radians(-90.0f));
}

void Scene::update()
{
	deleteEmptyBezierCurves();
	deleteInvalidBezierPatches();
	deleteInvalidGregorySurfaces();
	deleteUnreferencedNonDeletablePoints();
}

void Scene::render()
{
	if (m_renderHeightmap)
	{
		clearFramebuffer(AnaglyphMode::none);
		m_heightmap.bindTexture();
		ShaderPrograms::quad->use();
		ShaderPrograms::quad->setUniform("level", 0.0f);
		m_quad.render();
		return;
	}

	if (m_renderOffsetHeightmap)
	{
		clearFramebuffer(AnaglyphMode::none);
		m_offsetHeightmap.bindTexture();
		ShaderPrograms::quad->use();
		ShaderPrograms::quad->setUniform("level", baseHeight);
		m_quad.render();
		return;
	}

	if (m_renderEdge)
	{
		clearFramebuffer(AnaglyphMode::none);
		m_edge.bindTexture();
		ShaderPrograms::quad->use();
		ShaderPrograms::quad->setUniform("level", 0.0f);
		m_quad.render();
		return;
	}

	if (m_anaglyphOn)
	{
		m_leftEyeFramebuffer.bind();
		clearFramebuffer(AnaglyphMode::leftEye);

		m_activeCamera->useLeftEye();
		renderModels();
		renderCursor();
		renderSelectedModelsCenter();
		renderGrid();

		m_leftEyeFramebuffer.unbind();
		clearFramebuffer(AnaglyphMode::rightEye);

		m_activeCamera->useRightEye();
		renderModels();
		renderCursor();
		renderSelectedModelsCenter();
		renderGrid();

		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);
		m_leftEyeFramebuffer.bindTexture();
		ShaderPrograms::quad->use();
		m_quad.render();
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		clearFramebuffer(AnaglyphMode::none);

		m_activeCamera->use();
		renderModels();
		renderCursor();
		renderSelectedModelsCenter();
		renderGrid();
	}
}

void Scene::updateWindowSize(const glm::ivec2& windowSize)
{
	m_perspectiveCamera.updateWindowSize();
	m_orthographicCamera.updateWindowSize();
	m_leftEyeFramebuffer.resize(windowSize);
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

void Scene::moveCameraToSelectedModels()
{
	if (!isAnyModelSelected())
	{
		return;
	}

	glm::vec3 pos = m_selectedModelsCenter.getPos();
	m_perspectiveCamera.setTargetPos(pos);
	m_orthographicCamera.setTargetPos(pos);
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

		case ModelType::c0BezierCurve:
			return static_cast<int>(m_c0BezierCurves.size());

		case ModelType::c2BezierCurve:
			return static_cast<int>(m_c2BezierCurves.size());

		case ModelType::interpolatingBezierCurve:
			return static_cast<int>(m_interpolatingBezierCurves.size());

		case ModelType::bezierPatch:
			return static_cast<int>(m_bezierPatches.size());

		case ModelType::c0BezierSurface:
			return static_cast<int>(m_c0BezierSurfaces.size());

		case ModelType::c2BezierSurface:
			return static_cast<int>(m_c2BezierSurfaces.size());

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

		case ModelType::c0BezierCurve:
			return m_c0BezierCurves[i]->isVirtual();

		case ModelType::c2BezierCurve:
			return m_c2BezierCurves[i]->isVirtual();

		case ModelType::interpolatingBezierCurve:
			return m_interpolatingBezierCurves[i]->isVirtual();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->isVirtual();

		case ModelType::c0BezierSurface:
			return m_c0BezierSurfaces[i]->isVirtual();

		case ModelType::c2BezierSurface:
			return m_c2BezierSurfaces[i]->isVirtual();

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

		case ModelType::c0BezierCurve:
			return m_c0BezierCurves[i]->isSelected();

		case ModelType::c2BezierCurve:
			return m_c2BezierCurves[i]->isSelected();

		case ModelType::interpolatingBezierCurve:
			return m_interpolatingBezierCurves[i]->isSelected();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->isSelected();

		case ModelType::c0BezierSurface:
			return m_c0BezierSurfaces[i]->isSelected();

		case ModelType::c2BezierSurface:
			return m_c2BezierSurfaces[i]->isSelected();

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

		case ModelType::c0BezierCurve:
			m_c0BezierCurves[i]->select();
			m_selectedModels.push_back(m_c0BezierCurves[i].get());
			break;

		case ModelType::c2BezierCurve:
			m_c2BezierCurves[i]->select();
			m_selectedModels.push_back(m_c2BezierCurves[i].get());
			break;

		case ModelType::interpolatingBezierCurve:
			m_interpolatingBezierCurves[i]->select();
			m_selectedModels.push_back(m_interpolatingBezierCurves[i].get());
			break;

		case ModelType::bezierPatch:
			m_bezierPatches[i]->select();
			m_selectedModels.push_back(m_bezierPatches[i].get());
			break;

		case ModelType::c0BezierSurface:
			m_c0BezierSurfaces[i]->select();
			m_selectedModels.push_back(m_c0BezierSurfaces[i].get());
			break;

		case ModelType::c2BezierSurface:
			m_c2BezierSurfaces[i]->select();
			m_selectedModels.push_back(m_c2BezierSurfaces[i].get());
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

		case ModelType::c0BezierCurve:
			m_c0BezierCurves[i]->deselect();
			deselectedModel = m_c0BezierCurves[i].get();
			break;

		case ModelType::c2BezierCurve:
			m_c2BezierCurves[i]->deselect();
			deselectedModel = m_c2BezierCurves[i].get();
			break;

		case ModelType::interpolatingBezierCurve:
			m_interpolatingBezierCurves[i]->deselect();
			deselectedModel = m_interpolatingBezierCurves[i].get();
			break;

		case ModelType::bezierPatch:
			m_bezierPatches[i]->deselect();
			deselectedModel = m_bezierPatches[i].get();
			break;

		case ModelType::c0BezierSurface:
			m_c0BezierSurfaces[i]->deselect();
			deselectedModel = m_c0BezierSurfaces[i].get();
			break;

		case ModelType::c2BezierSurface:
			m_c2BezierSurfaces[i]->deselect();
			deselectedModel = m_c2BezierSurfaces[i].get();
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
	deleteSelectedModels(m_c0BezierCurves);
	deleteSelectedModels(m_c2BezierCurves);
	deleteSelectedModels(m_interpolatingBezierCurves);
	deleteSelectedModels(m_bezierPatches);
	deleteSelectedModels(m_c0BezierSurfaces);
	deleteSelectedModels(m_c2BezierSurfaces);
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

void Scene::moveUniqueSelectedModel(const glm::vec2& offset) const
{
	Model* selectedModel = getUniqueSelectedModel();
	if (selectedModel != nullptr)
	{
		glm::vec3 pos = selectedModel->getPos();
		glm::vec2 screenPos = m_activeCamera->posToScreenPos(pos);
		glm::vec3 newPos = m_activeCamera->screenPosToPos(pos, screenPos + offset);
		selectedModel->setPos(newPos);
	}
}

void Scene::collapse2Points()
{
	std::vector<Point*> points = getNonVirtualSelectedPoints();
	if (m_selectedModels.size() != points.size() || points.size() != 2)
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

Intersectable* Scene::getUniqueSelectedIntersectable() const
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

	for (const std::unique_ptr<C0BezierSurface>& surface : m_c0BezierSurfaces)
	{
		if (surface->isSelected())
		{
			return surface.get();
		}
	}

	for (const std::unique_ptr<C2BezierSurface>& surface : m_c2BezierSurfaces)
	{
		if (surface->isSelected())
		{
			return surface.get();
		}
	}

	return nullptr;
}

bool Scene::isCursorAtPos(const glm::vec2& screenPos) const
{
	static constexpr float treshold = 30;
	glm::vec2 cursorScreenPos = m_activeCamera->posToScreenPos(m_cursor.getPos());
	glm::vec2 relativeScreenPos = cursorScreenPos - screenPos;
	float screenDistanceSquared = glm::dot(relativeScreenPos, relativeScreenPos);
	return screenDistanceSquared < treshold * treshold;
}

void Scene::moveCursor(const glm::vec2& offset)
{
	glm::vec3 pos = m_cursor.getPos();
	glm::vec2 screenPos = m_activeCamera->posToScreenPos(pos);
	glm::vec3 newPos = m_activeCamera->screenPosToPos(pos, screenPos + offset);
	m_cursor.setPos(newPos);
}

void Scene::moveCursorToSelectedModels()
{
	if (!isAnyModelSelected())
	{
		return;
	}

	glm::vec3 pos = m_selectedModelsCenter.getPos();
	m_cursor.setPos(pos);
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

		case ModelType::c0BezierCurve:
			return m_c0BezierCurves[i]->getOriginalName();

		case ModelType::c2BezierCurve:
			return m_c2BezierCurves[i]->getOriginalName();

		case ModelType::interpolatingBezierCurve:
			return m_interpolatingBezierCurves[i]->getOriginalName();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->getOriginalName();

		case ModelType::c0BezierSurface:
			return m_c0BezierSurfaces[i]->getOriginalName();

		case ModelType::c2BezierSurface:
			return m_c2BezierSurfaces[i]->getOriginalName();

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

		case ModelType::c0BezierCurve:
			return m_c0BezierCurves[i]->getName();

		case ModelType::c2BezierCurve:
			return m_c2BezierCurves[i]->getName();

		case ModelType::interpolatingBezierCurve:
			return m_interpolatingBezierCurves[i]->getName();

		case ModelType::bezierPatch:
			return m_bezierPatches[i]->getName();

		case ModelType::c0BezierSurface:
			return m_c0BezierSurfaces[i]->getName();

		case ModelType::c2BezierSurface:
			return m_c2BezierSurfaces[i]->getName();

		case ModelType::gregorySurface:
			return m_gregorySurfaces[i]->getName();

		case ModelType::intersectionCurve:
			return m_intersectionCurves[i]->getName();
	}
	return {};
}

void Scene::addPoint()
{
	std::unique_ptr<Point> point = std::make_unique<Point>(m_cursor.getPos());

	if (m_selectedModels.size() == 1)
	{
		auto selectedC0BezierCurve = std::find_if
		(
			m_c0BezierCurves.begin(), m_c0BezierCurves.end(),
			[] (const std::unique_ptr<C0BezierCurve>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedC0BezierCurve != m_c0BezierCurves.end())
		{
			(*selectedC0BezierCurve)->addPoints({point.get()});
		}

		auto selectedC2BezierCurve = std::find_if
		(
			m_c2BezierCurves.begin(), m_c2BezierCurves.end(),
			[] (const std::unique_ptr<C2BezierCurve>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedC2BezierCurve != m_c2BezierCurves.end())
		{
			std::vector<std::unique_ptr<Point>> newPoints{};
			(*selectedC2BezierCurve)->addPoints({point.get()}, newPoints);
			addPoints(std::move(newPoints));
		}

		auto selectedInterpolatingBezierCurve = std::find_if
		(
			m_interpolatingBezierCurves.begin(), m_interpolatingBezierCurves.end(),
			[] (const std::unique_ptr<InterpolatingBezierCurve>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedInterpolatingBezierCurve != m_interpolatingBezierCurves.end())
		{
			(*selectedInterpolatingBezierCurve)->addPoints({point.get()});
		}
	}

	m_models.push_back(point.get());
	m_points.push_back(std::move(point));
}

void Scene::addTorus()
{
	std::unique_ptr<Torus> torus = std::make_unique<Torus>(
		[this] (const std::vector<IntersectionCurve*>& intersectionCurves)
		{
			deleteIntersectionCurves(intersectionCurves);
		}, m_cursor.getPos());
	m_models.push_back(torus.get());
	m_toruses.push_back(std::move(torus));
}

void Scene::addC0BezierCurve()
{
	std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
	if (m_selectedModels.size() != nonVirtualSelectedPoints.size() ||
		nonVirtualSelectedPoints.size() == 0)
	{
		return;
	}

	std::unique_ptr<C0BezierCurve> curve = std::make_unique<C0BezierCurve>(nonVirtualSelectedPoints,
		m_bezierCurveSelfDestructCallback);
	m_models.push_back(curve.get());
	m_c0BezierCurves.push_back(std::move(curve));
}

void Scene::addC2BezierCurve()
{
	std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
	if (m_selectedModels.size() != nonVirtualSelectedPoints.size() ||
		nonVirtualSelectedPoints.size() == 0)
	{
		return;
	}

	std::vector<std::unique_ptr<Point>> newPoints{};
	std::unique_ptr<C2BezierCurve> curve = std::make_unique<C2BezierCurve>(nonVirtualSelectedPoints,
		m_bezierCurveSelfDestructCallback, newPoints);
	m_models.push_back(curve.get());
	m_c2BezierCurves.push_back(std::move(curve));
	addPoints(std::move(newPoints));
}

void Scene::addInterpolatingBezierCurve()
{
	std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
	if (m_selectedModels.size() != nonVirtualSelectedPoints.size() ||
		nonVirtualSelectedPoints.size() == 0)
	{
		return;
	}

	std::unique_ptr<InterpolatingBezierCurve> curve = std::make_unique<InterpolatingBezierCurve>(
		nonVirtualSelectedPoints, m_bezierCurveSelfDestructCallback);
	m_models.push_back(curve.get());
	m_interpolatingBezierCurves.push_back(std::move(curve));
}

void Scene::addSelectedPointsToCurve()
{
	auto selectedC0BezierCurve = std::find_if
	(
		m_c0BezierCurves.begin(), m_c0BezierCurves.end(),
		[] (const std::unique_ptr<C0BezierCurve>& curve)
		{
			return curve->isSelected();
		}
	);
	if (selectedC0BezierCurve != m_c0BezierCurves.end())
	{
		std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
		if (m_selectedModels.size() == nonVirtualSelectedPoints.size() + 1 &&
			nonVirtualSelectedPoints.size() != 0)
		{
			(*selectedC0BezierCurve)->addPoints(nonVirtualSelectedPoints);
		}
	}

	auto selectedC2BezierCurve = std::find_if
	(
		m_c2BezierCurves.begin(), m_c2BezierCurves.end(),
		[] (const std::unique_ptr<C2BezierCurve>& curve)
		{
			return curve->isSelected();
		}
	);
	if (selectedC2BezierCurve != m_c2BezierCurves.end())
	{
		std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
		if (m_selectedModels.size() == nonVirtualSelectedPoints.size() + 1 &&
			nonVirtualSelectedPoints.size() != 0)
		{
			std::vector<std::unique_ptr<Point>> newPoints{};
			(*selectedC2BezierCurve)->addPoints(nonVirtualSelectedPoints, newPoints);
			addPoints(std::move(newPoints));
		}
	}

	auto selectedInterpolatingBezierCurve = std::find_if
	(
		m_interpolatingBezierCurves.begin(), m_interpolatingBezierCurves.end(),
		[] (const std::unique_ptr<InterpolatingBezierCurve>& curve)
		{
			return curve->isSelected();
		}
	);
	if (selectedInterpolatingBezierCurve != m_interpolatingBezierCurves.end())
	{
		std::vector<Point*> nonVirtualSelectedPoints = getNonVirtualSelectedPoints();
		if (m_selectedModels.size() == nonVirtualSelectedPoints.size() + 1 &&
			nonVirtualSelectedPoints.size() != 0)
		{
			(*selectedInterpolatingBezierCurve)->addPoints(nonVirtualSelectedPoints);
		}
	}
}

void Scene::addC0BezierSurface(int patchesU, int patchesV, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping)
{
	std::vector<std::unique_ptr<Point>> newPoints{};
	std::vector<std::unique_ptr<BezierPatch>> newPatches{};
	std::unique_ptr<C0BezierSurface> surface = std::make_unique<C0BezierSurface>(
		[this] (const std::vector<IntersectionCurve*>& intersectionCurves)
		{
			deleteIntersectionCurves(intersectionCurves);
		},
		patchesU, patchesV, m_cursor.getPos(), sizeU, sizeV, wrapping, newPoints, newPatches);
	addPoints(std::move(newPoints));
	addBezierPatches(std::move(newPatches));
	m_models.push_back(surface.get());
	m_c0BezierSurfaces.push_back(std::move(surface));
}

void Scene::addC2BezierSurface(int patchesU, int patchesV, float sizeU, float sizeV,
	BezierSurfaceWrapping wrapping)
{
	std::vector<std::unique_ptr<Point>> newPoints{};
	std::vector<std::unique_ptr<BezierPatch>> newPatches{};
	std::unique_ptr<C2BezierSurface> surface = std::make_unique<C2BezierSurface>(
		[this] (const std::vector<IntersectionCurve*>& intersectionCurves)
		{
			deleteIntersectionCurves(intersectionCurves);
		},
		patchesU, patchesV, m_cursor.getPos(), sizeU, sizeV, wrapping, newPoints, newPatches);
	addPoints(std::move(newPoints));
	addBezierPatches(std::move(newPatches));
	m_models.push_back(surface.get());
	m_c2BezierSurfaces.push_back(std::move(surface));
}

void Scene::addGregorySurface(const std::array<BezierPatch*, 3>& patches)
{
	std::unique_ptr<GregorySurface> surface = GregorySurface::create(patches,
		m_gregorySurfaceSelfDestructCallback);

	if (surface != nullptr)
	{
		m_models.push_back(surface.get());
		m_gregorySurfaces.push_back(std::move(surface));
	}
}

void Scene::addIntersectionCurve(const std::array<Intersectable*, 2>& surfaces, float step,
	bool useCursor)
{
	std::unique_ptr<IntersectionCurve> intersectionCurve{};
	if (useCursor)
	{
		intersectionCurve = IntersectionCurve::create({surfaces[0], surfaces[1]}, step,
			m_cursor.getPos());
	}
	else
	{
		intersectionCurve = IntersectionCurve::create({surfaces[0], surfaces[1]}, step);
	}

	if (intersectionCurve != nullptr)
	{
		surfaces[0]->addIntersectionCurve(intersectionCurve.get(), 0);
		if (surfaces[0] != surfaces[1])
		{
			surfaces[1]->addIntersectionCurve(intersectionCurve.get(), 1);
		}

		m_models.push_back(intersectionCurve.get());
		m_intersectionCurves.push_back(std::move(intersectionCurve));
	}
}

void Scene::convertIntersectionToInterpolatingCurve(int numberOfPoints)
{
	if (m_selectedModels.size() != 1)
	{
		return;
	}

	auto selectedIntersectionCurve = std::find_if
	(
		m_intersectionCurves.begin(), m_intersectionCurves.end(),
		[] (const std::unique_ptr<IntersectionCurve>& curve)
		{
			return curve->isSelected();
		}
	);

	if (selectedIntersectionCurve == m_intersectionCurves.end())
	{
		return;
	}

	std::vector<glm::vec3> intersectionPoints =
		(*selectedIntersectionCurve)->getIntersectionPoints();
	bool isClosed = (*selectedIntersectionCurve)->isClosed();
	int segments = isClosed ? numberOfPoints : numberOfPoints - 1;

	std::vector<Point*> points{};
	float stride = (static_cast<float>(intersectionPoints.size()) - 1) / segments;
	for (int i = 0; i < segments; ++i)
	{
		std::unique_ptr<Point> point = std::make_unique<Point>(
			intersectionPoints[static_cast<std::size_t>(i * stride)]);
		points.push_back(point.get());
		m_models.push_back(point.get());
		m_points.push_back(std::move(point));
	}

	if (isClosed)
	{
		points.push_back(points[0]);
	}
	else
	{
		std::unique_ptr<Point> lastPoint = std::make_unique<Point>(intersectionPoints.back());
		points.push_back(lastPoint.get());
		m_models.push_back(lastPoint.get());
		m_points.push_back(std::move(lastPoint));
	}

	std::unique_ptr<InterpolatingBezierCurve> curve = std::make_unique<InterpolatingBezierCurve>(
		points, m_bezierCurveSelfDestructCallback);
	m_models.push_back(curve.get());
	m_interpolatingBezierCurves.push_back(std::move(curve));
}

void Scene::updateActiveCameraGUI()
{
	m_activeCamera->updateGUI();
}

void Scene::updateCursorGUI()
{
	m_cursor.updateGUI(*m_activeCamera);
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

		case ModelType::c0BezierCurve:
			m_c0BezierCurves[i]->updateGUI();
			break;

		case ModelType::c2BezierCurve:
			m_c2BezierCurves[i]->updateGUI();
			break;

		case ModelType::interpolatingBezierCurve:
			m_interpolatingBezierCurves[i]->updateGUI();
			break;

		case ModelType::bezierPatch:
			m_bezierPatches[i]->updateGUI();
			break;

		case ModelType::c0BezierSurface:
			m_c0BezierSurfaces[i]->updateGUI();
			break;

		case ModelType::c2BezierSurface:
			m_c2BezierSurfaces[i]->updateGUI();
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

void Scene::magic()
{
	for (auto& point : m_points)
	{
		auto pos = point->getPos();
		pos.x *= 0.977f;
		pos.x += 0.158f;
		pos.x *= 0.998f;
		pos.z += 0.62f;
		point->setPos(pos);
	}
}

void Scene::generateHeightmap()
{
	m_heightmap.bind();
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_heightmapCamera.use();
	for (const auto& surface : m_bezierPatches)
	{
		surface->render();
	}
	m_heightmap.unbind();
	m_renderHeightmap = true;
	glViewport(LeftPanel::width, 0, heightmapSize.x, heightmapSize.y);
}

void Scene::generateOffsetHeightmap(float radius, bool flatCutter, float pathLevel)
{
	m_offsetHeightmap.bind();
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderPrograms::heightmap->use();
	ShaderPrograms::heightmap->setUniform("radius", radius);
	ShaderPrograms::heightmap->setUniform("flatCutter", flatCutter);
	ShaderPrograms::heightmap->setUniform("base", baseHeight);
	ShaderPrograms::heightmap->setUniform("pathLevel", pathLevel);
	m_heightmap.bindTexture();
	m_quad.render();
	m_offsetHeightmap.unbind();
	m_renderHeightmap = false;
	m_renderOffsetHeightmap = true;
}

void Scene::generateEdge()
{
	m_edge.bind();
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderPrograms::edge->use();
	ShaderPrograms::edge->setUniform("level", baseHeight);
	ShaderPrograms::edge->setUniform("resolution", heightmapSize);
	m_offsetHeightmap.bindTexture();
	m_quad.render();
	m_edge.unbind();
	m_renderHeightmap = false;
	m_renderOffsetHeightmap = false;
	m_renderEdge = true;
}

std::unique_ptr<Scene::HeightmapData> Scene::getOffsetHeightmapData()
{
	auto textureData = std::make_unique<TextureData>();
	m_offsetHeightmap.bind();
	m_offsetHeightmap.getTextureData((*textureData)[0][0].data());
	m_offsetHeightmap.unbind();
	auto offsetHeightmapData = std::make_unique<HeightmapData>();
	for (int i = 0; i < heightmapSize.y; ++i)
	{
		for (int j = 0; j < heightmapSize.x; ++j)
		{
			(*offsetHeightmapData)[i][j] = (*textureData)[i][j][0];
		}
	}
	return offsetHeightmapData;
}

float Scene::getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
	int xIndex, float z)
{
	if (xIndex < 0 || xIndex >= heightmapSize.x)
	{
		return defaultHeight;
	}
	float yPix = (z + 7.5f) / 15.0f * heightmapSize.y - 0.5f;
	if (yPix < 0 || yPix >= heightmapSize.y - 1)
	{
		return defaultHeight;
	}
	int yPixFloor = static_cast<int>(glm::floor(yPix));
	int yPixCeil = yPixFloor + 1;
	float heightYFloor = heightmapData[yPixFloor][xIndex];
	float heightYCeil = heightmapData[yPixCeil][xIndex];
	return (yPix - yPixFloor) * heightYCeil + (yPixCeil - yPix) * heightYFloor;
}

void Scene::generatePath1()
{
	constexpr float zStart = -7.6f;
	constexpr float xOffset = path1Radius * 1.2f;

	auto generate = [this] (std::vector<glm::vec3>& path, float pathLevel,
		bool backwards, bool left)
		{
			auto offsetHeightmapData = getOffsetHeightmapData();

			constexpr float stride = path1Radius;
			constexpr int passCount = 20;
			float lowestHeight = baseHeight + pathLevel + path1Offset;

			auto getHeight = [this, lowestHeight, &offsetHeightmapData] (int xIndex, float z)
				{
					return getHeightmapHeight(lowestHeight, *offsetHeightmapData, xIndex, z);
				};

			auto xIndex2X = [] (int xIndex)
				{
					return (xIndex + 0.5f) / heightmapSize.x * 15.0f - 7.5f;
				};

			float dx = 15.0f / heightmapSize.x;

			for (int i = 0; i < passCount; ++i)
			{
				float xStart = -7.5 - xOffset;
				float xEnd = 7.5 + xOffset;
				if (left)
				{
					std::swap(xStart, xEnd);
				}
				float z = zStart + (backwards ? passCount - 1 - i : i) * stride;
				path.push_back({xStart, lowestHeight, z});

				float xPrev = xStart;
				float yPrev = lowestHeight;

				float heightPrevPix = getHeight(left ? heightmapSize.x : -1, z);
				float heightCurrPix = getHeight(left ? heightmapSize.x - 1 : 0, z);

				for (int xIndex = 0; xIndex < heightmapSize.x; ++xIndex)
				{
					float heightNextPix =
						getHeight(left ? heightmapSize.x - 2 - xIndex : xIndex + 1, z);

					float firstDeriv = (heightNextPix - heightPrevPix) / (2.0f * dx);
					float secondDeriv =
						(heightNextPix - 2 * heightCurrPix + heightPrevPix) / (dx * dx);

					constexpr float eps = 1e-12f;
					float curvatureRadius = std::pow(1 + firstDeriv * firstDeriv, 1.5f) /
						(std::abs(secondDeriv) + eps);

					float x = xIndex2X(left ? heightmapSize.x - 1 - xIndex : xIndex);
					float segmentLengthSquared =
						std::pow(x - xPrev, 2.0f) + std::pow(heightCurrPix - yPrev, 2.0f);
					if (segmentLengthSquared > 4 * (2 * curvatureRadius * path1SegmentingOffset -
						std::pow(path1SegmentingOffset, 2)))
					{
						xPrev = xIndex2X(left ? heightmapSize.x - xIndex : xIndex - 1);
						yPrev = heightPrevPix;
						path.push_back({xPrev, yPrev, z});
					}

					heightPrevPix = heightCurrPix;
					heightCurrPix = heightNextPix;
				}

				path.push_back({xEnd, lowestHeight, z});

				left = !left;
			}
		};

	std::vector<glm::vec3> path{};
	path.push_back({0, yDefault + path1Radius, 0});
	path.push_back({-7.5f - xOffset, yDefault + path1Radius, zStart});
	
	generateOffsetHeightmap(path1Offset, false, path1IntermediateLevel);
	generate(path, path1IntermediateLevel, false, false);
	generateOffsetHeightmap(path1Offset, false);
	generate(path, 0, true, false);

	path.push_back({-7.5f - xOffset, yDefault + path1Radius, zStart});
	path.push_back({0, yDefault + path1Radius, 0});

	std::ofstream file{"D:/Desktop/VisualStudio/IiSI/3c-milling-simulator/res/toolpaths/p1.k16"};
	for (int i = 0; i < path.size(); ++i)
	{
		file << std::format("N{}G01X{:.3f}Y{:.3f}Z{:.3f}\n", i + 3, path[i].x * 10, path[i].z * 10,
			(path[i].y - path1Radius) * 10);
	}
	file.close();
}

void Scene::generatePath2()
{
	constexpr float zStart = -7.6f;
	constexpr float xOffset = path2Radius * 1.2f;
	constexpr float stride = 1.9f * path2Radius;
	constexpr int passCount = 16;

	auto generate = [this] (std::vector<glm::vec3>& path, bool backwards)
		{
			auto offsetHeightmapData = getOffsetHeightmapData();

			constexpr float dz = 15.0f / heightmapSize.y;
			constexpr int stridePix = stride / dz + 1;

			auto getHeight = [this, &offsetHeightmapData] (int xIndex, float z)
				{
					return getHeightmapHeight(baseHeight, *offsetHeightmapData, xIndex, z);
				};

			auto xIndex2X = [] (int xIndex)
				{
					return (xIndex + 0.5f) / heightmapSize.x * 15.0f - 7.5f;
				};

			auto getLastZero = [backwards, getHeight] (float z)
				{
					int xIndex = backwards ? heightmapSize.x - 1 : 0;
					int increment = backwards ? -1 : 1;
					for (int i = 0; i <= heightmapSize.x / 2; ++i)
					{
						xIndex += increment;
						constexpr float eps = 1e-9f;
						if (getHeight(xIndex, z) > baseHeight + eps)
						{
							break;
						}
					}
					return xIndex - increment;
				};

			for (int i = 0; i < passCount; i += 2)
			{
				float xStart = -7.5f - xOffset;

				if (backwards)
				{
					xStart = -xStart;
				}

				float z = zStart + (backwards ? passCount - 1 - i : i) * stride;
				float z1 = z;
				float z2 = z + (backwards ? -stride : stride);
				path.push_back({xStart, baseHeight, z1});

				int xIndex1 = getLastZero(z1);
				float x1 = xIndex2X(xIndex1);
				float xLine1 = x1;
				int xIndex2 = getLastZero(z2);
				float x2 = xIndex2X(xIndex2);
				float xLine2 = x2;

				float angle = std::atan((x2 - x1) / (backwards ? -stride : stride));
				constexpr float maxAngle = glm::radians(45.0f);

				if (angle < -maxAngle)
				{
					path.push_back({x1, baseHeight, z1});
					xLine1 = xLine2 + (backwards ? -stride : stride) * std::tan(maxAngle);
				}
				if (angle > maxAngle)
				{
					xLine2 = xLine1 + (backwards ? -stride : stride) * std::tan(maxAngle);
				}

				float maxXOffset = 0;
				for (int j = 0; j < stridePix; ++j)
				{
					float zOffset = j * dz;
					int xIndex = getLastZero(z1 + (backwards ? -zOffset : zOffset));
					float x = xIndex2X(xIndex);
					float xLine = xLine1 + (xLine2 - xLine1) * zOffset / stride;
					maxXOffset = std::max(maxXOffset, (backwards ? x - xLine : xLine - x));
				}
				path.push_back({xLine1 + (backwards ? maxXOffset : -maxXOffset), baseHeight, z1});
				path.push_back({xLine2 + (backwards ? maxXOffset : -maxXOffset), baseHeight, z2});

				if (angle > maxAngle)
				{
					path.push_back({x2, baseHeight, z2});
				}

				path.push_back({xStart, baseHeight, z2});
			}
		};

	std::vector<glm::vec3> path{};
	path.push_back({0, yDefault, 0});
	path.push_back({-7.5f - xOffset, yDefault, zStart});
	
	generateOffsetHeightmap(path2Radius, true);
	generate(path, false);
	path.push_back({-7.5f - xOffset, baseHeight, zStart + passCount * stride});
	path.push_back({7.5f + xOffset, baseHeight, zStart + passCount * stride});
	generate(path, true);

	path.push_back({7.5f + xOffset, yDefault, zStart});
	path.push_back({0, yDefault, 0});

	std::ofstream file{"D:/Desktop/VisualStudio/IiSI/3c-milling-simulator/res/toolpaths/p2.f10"};
	for (int i = 0; i < path.size(); ++i)
	{
		file << std::format("N{}G01X{:.3f}Y{:.3f}Z{:.3f}\n", i + 3, path[i].x * 10, path[i].z * 10,
			path[i].y * 10);
	}
	file.close();
}

void Scene::setUpFramebuffer() const
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_MULTISAMPLE);
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

void Scene::renderModels() const
{
	for (const Model* model : m_models)
	{
		model->render();
	}
}

void Scene::renderCursor() const
{
	m_cursor.render();
}

void Scene::renderSelectedModelsCenter() const
{
	m_selectedModelsCenter.render();
}

void Scene::renderGrid() const
{
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
	for (int i = 0; i < m_models.size(); ++i)
	{
		glm::vec2 modelScreenPos = m_activeCamera->posToScreenPos(m_models[i]->getPos());
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
	for (const Model* model : m_selectedModels)
	{
		auto point = find_if
		(
			m_points.begin(), m_points.end(),
			[model] (const std::unique_ptr<Point>& point)
			{
				return point.get() == model;
			}
		);
		if (point != m_points.end() && !(*point)->isVirtual())
		{
			selectedPoints.push_back(point->get());
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
			m_c0BezierCurves,
			[curve] (const std::unique_ptr<C0BezierCurve>& c0Curve)
			{
				return c0Curve.get() == curve;
			}
		);
		std::erase_if
		(
			m_c2BezierCurves,
			[curve] (const std::unique_ptr<C2BezierCurve>& c2Curve)
			{
				return c2Curve.get() == curve;
			}
		);
		std::erase_if
		(
			m_interpolatingBezierCurves,
			[curve] (const std::unique_ptr<InterpolatingBezierCurve>& interpolatingCurve)
			{
				return interpolatingCurve.get() == curve;
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

void Scene::deleteIntersectionCurves(const std::vector<IntersectionCurve*>& intersectionCurves)
{
	for (const IntersectionCurve* curve : intersectionCurves)
	{
		std::erase(m_models, curve);
		std::erase(m_selectedModels, curve);
		std::erase_if(m_intersectionCurves,
			[curve] (const std::unique_ptr<IntersectionCurve>& intersectionCurve)
			{
				return intersectionCurve.get() == curve;
			}
		);
	}
}
