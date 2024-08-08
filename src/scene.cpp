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
	m_cursor{},
	m_perspectiveCamera{fovYDeg, static_cast<float>(windowSize.x) / windowSize.y, nearPlane,
		farPlane, m_shaderPrograms},
	m_orthographicCamera{viewHeight, static_cast<float>(windowSize.x) / windowSize.y, nearPlane,
		farPlane, m_shaderPrograms}
{
	auto firstModelIter = m_models.begin();

	setCameraType(m_cameraType);
	zoomCamera(0.5);
	addPitchCamera(glm::radians(-30.0f));
	addYawCamera(glm::radians(15.0f));
}

void Scene::render()
{
	m_activeCamera->use(m_windowSize);
	renderModels();
	renderCursor();
	renderSelectedModelsCenter();
	renderGrid();
}

void Scene::updateWindowSize()
{
	setAspectRatio(static_cast<float>(m_windowSize.x) / m_windowSize.y);
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

int Scene::getModelCount() const
{
	return static_cast<int>(m_models.size());
}

bool Scene::isAnyModelSelected() const
{
	return m_selectedModels.size() > 0;
}

bool Scene::isOneModelSelected() const
{
	return m_selectedModels.size() == 1;
}

bool Scene::isModelVirtual(int i) const
{
	return m_models[i]->isVirtual();
}

bool Scene::isModelSelected(int i) const
{
	return m_models[i]->isSelected();
}

void Scene::selectModel(int i)
{
	m_models[i]->select();
	m_selectedModels.push_back(m_models[i]);
}

void Scene::deselectModel(int i)
{
	m_models[i]->deselect();
	std::erase_if(m_selectedModels,
		[deletedModel = m_models[i]] (Model* model)
		{
			return model == deletedModel;
		}
	);
}

void Scene::toggleModel(int i)
{
	if (isModelSelected(i))
	{
		deselectModel(i);
	}
	else
	{
		selectModel(i);
	}
}

void Scene::deselectAllModels()
{
	for (Model* model : m_models)
	{
		model->deselect();
	}
	m_selectedModels.clear();
}

void Scene::deleteSelectedModels()
{
	std::erase_if(m_models,
		[] (Model* model)
		{
			return model->isSelected() && !model->isVirtual();
		}
	);
	std::erase_if(m_points,
		[] (const std::unique_ptr<Point>& point)
		{
			return point->isSelected() && !point->isVirtual();
		}
	);
	std::erase_if(m_toruses,
		[] (const std::unique_ptr<Torus>& torus)
		{
			return torus->isSelected() && !torus->isVirtual();
		}
	);
	std::erase_if(m_bezierCurvesC0,
		[] (const std::unique_ptr<BezierCurveC0>& curve)
		{
			return curve->isSelected() && !curve->isVirtual();
		}
	);
	std::erase_if(m_bezierCurvesC2,
		[] (const std::unique_ptr<BezierCurveC2>& curve)
		{
			return curve->isSelected() && !curve->isVirtual();
		}
	);
	std::erase_if(m_bezierCurvesInter,
		[] (const std::unique_ptr<BezierCurveInter>& curve)
		{
			return curve->isSelected() && !curve->isVirtual();
		}
	);
	
	deleteEmptyBezierCurvesC0();
	deleteEmptyBezierCurvesC2();
	deleteEmptyBezierCurvesInter();
	deleteUnreferencedVirtualPoints();

	m_selectedModels.clear();
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

std::string Scene::getModelName(int i) const
{
	return m_models[i]->getName();
}

void Scene::addPoint()
{
	std::unique_ptr<Point> point = std::make_unique<Point>(m_shaderPrograms.point,
		m_cursor.getPos());

	if (m_selectedModels.size() == 1)
	{
		auto selectedBezierCurveC0 = std::find_if(m_bezierCurvesC0.begin(), m_bezierCurvesC0.end(),
			[] (const std::unique_ptr<BezierCurveC0>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedBezierCurveC0 != m_bezierCurvesC0.end())
		{
			(*selectedBezierCurveC0)->addPoints({point.get()});
		}

		auto selectedBezierCurveC2 = std::find_if(m_bezierCurvesC2.begin(), m_bezierCurvesC2.end(),
			[] (const std::unique_ptr<BezierCurveC2>& curve)
			{
				return curve->isSelected();
			}
		);

		if (selectedBezierCurveC2 != m_bezierCurvesC2.end())
		{
			addVirtualPoints((*selectedBezierCurveC2)->addPoints({point.get()}));
		}

		auto selectedBezierCurveInter = std::find_if(m_bezierCurvesInter.begin(),
			m_bezierCurvesInter.end(),
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
	std::unique_ptr<Torus> torus = std::make_unique<Torus>(m_shaderPrograms.torus,
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
		m_shaderPrograms.bezierCurve, m_shaderPrograms.bezierCurvePolyline,
		nonVirtualSelectedPoints);
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

	auto [curve, virtualPoints] = BezierCurveC2::create(m_shaderPrograms.bezierCurve,
		m_shaderPrograms.bezierCurvePolyline, m_shaderPrograms.point, nonVirtualSelectedPoints);
	m_models.push_back(curve.get());
	m_bezierCurvesC2.push_back(std::move(curve));
	addVirtualPoints(std::move(virtualPoints));
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
		m_shaderPrograms.bezierCurveInter, m_shaderPrograms.bezierCurvePolyline,
		nonVirtualSelectedPoints);
	m_models.push_back(curve.get());
	m_bezierCurvesInter.push_back(std::move(curve));
}

void Scene::addSelectedPointsToCurve()
{
	auto selectedBezierCurveC0 = std::find_if(m_bezierCurvesC0.begin(), m_bezierCurvesC0.end(),
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

	auto selectedBezierCurveC2 = std::find_if(m_bezierCurvesC2.begin(), m_bezierCurvesC2.end(),
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
			addVirtualPoints((*selectedBezierCurveC2)->addPoints(nonVirtualSelectedPoints));
		}
	}

	auto selectedBezierCurveInter = std::find_if(m_bezierCurvesInter.begin(),
		m_bezierCurvesInter.end(),
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

void Scene::updateModelGUI(int i)
{
	m_models[i]->updateGUI();
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
	m_cursor.render(m_shaderPrograms.cursor);
}

void Scene::renderSelectedModelsCenter()
{
	m_shaderPrograms.cursor.use();
	m_selectedModelsCenter.render(m_shaderPrograms.cursor);
}

void Scene::renderGrid() const
{
	m_shaderPrograms.grid.use();
	m_grid.render(m_shaderPrograms.grid, m_cameraType);
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
	for (const Model* model : m_selectedModels)
	{
		if (model->isVirtual())
		{
			continue;
		}
		auto nonVirtualSelectedPoint = std::find_if(m_points.begin(), m_points.end(),
			[model] (const std::unique_ptr<Point>& point)
			{
				return model == point.get();
			}
		);
		if (nonVirtualSelectedPoint != m_points.end())
		{
			selectedPoints.push_back(nonVirtualSelectedPoint->get());
		}
	}
	return selectedPoints;
}

void Scene::addVirtualPoints(std::vector<std::unique_ptr<Point>> points)
{
	for (const std::unique_ptr<Point>& point : points)
	{
		m_models.push_back(point.get());
	}
	m_points.insert(m_points.end(), std::make_move_iterator(points.begin()),
		std::make_move_iterator(points.end()));
}

void Scene::deleteEmptyBezierCurvesC0()
{
	std::vector<BezierCurveC0*> bezierCurvesC0ToBeDeleted{};
	std::erase_if(m_bezierCurvesC0,
		[&bezierCurvesC0ToBeDeleted] (const std::unique_ptr<BezierCurveC0>& curve)
		{
			if (curve->getPointCount() == 0)
			{
				bezierCurvesC0ToBeDeleted.push_back(curve.get());
				return true;
			}
			return false;
		}
	);
	for (const BezierCurveC0* curve : bezierCurvesC0ToBeDeleted)
	{
		std::erase_if(m_models,
			[&curve] (Model* model)
			{
				return model == curve;
			}
		);
	}
}

void Scene::deleteEmptyBezierCurvesC2()
{
	std::vector<BezierCurveC2*> bezierCurvesC2ToBeDeleted{};
	std::erase_if(m_bezierCurvesC2,
		[&bezierCurvesC2ToBeDeleted] (const std::unique_ptr<BezierCurveC2>& curve)
		{
			if (curve->getPointCount() == 0)
			{
				bezierCurvesC2ToBeDeleted.push_back(curve.get());
				return true;
			}
			return false;
		}
	);
	for (const BezierCurveC2* curve : bezierCurvesC2ToBeDeleted)
	{
		std::erase_if(m_models,
			[&curve] (Model* model)
			{
				return model == curve;
			}
		);
	}
}

void Scene::deleteEmptyBezierCurvesInter()
{
	std::vector<BezierCurveInter*> bezierCurvesInterToBeDeleted{};
	std::erase_if(m_bezierCurvesInter,
		[&bezierCurvesInterToBeDeleted] (const std::unique_ptr<BezierCurveInter>& curve)
		{
			if (curve->getPointCount() == 0)
			{
				bezierCurvesInterToBeDeleted.push_back(curve.get());
				return true;
			}
			return false;
		}
	);
	for (const BezierCurveInter* curve : bezierCurvesInterToBeDeleted)
	{
		std::erase_if(m_models,
			[&curve] (Model* model)
			{
				return model == curve;
			}
		);
	}
}

void Scene::deleteUnreferencedVirtualPoints()
{
	std::vector<Point*> pointsToBeDeleted{};
	std::erase_if(m_points,
		[&pointsToBeDeleted] (const std::unique_ptr<Point>& point)
		{
			if (point->isVirtual() && !point->isReferenced())
			{
				pointsToBeDeleted.push_back(point.get());
				return true;
			}
			return false;
		}
	);
	for (const Point* point : pointsToBeDeleted)
	{
		std::erase_if(m_models,
			[&point] (Model* model)
			{
				return model == point;
			}
		);
	}
}
