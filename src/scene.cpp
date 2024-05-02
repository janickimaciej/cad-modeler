#include "scene.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>

constexpr float viewWidth = 20.0f;
constexpr float fovYDeg = 60.0f;
constexpr float nearPlane = 0.1f;
constexpr float farPlane = 1000.0f;

Scene::Scene(int windowWidth, int windowHeight) :
	m_windowWidth{windowWidth},
	m_windowHeight{windowHeight},
	m_cursor{*this},
	m_perspectiveCamera{fovYDeg, static_cast<float>(windowWidth) / windowHeight, nearPlane,
		farPlane, m_shaderPrograms},
	m_orthographicCamera{viewWidth, static_cast<float>(windowWidth) / windowHeight, nearPlane,
		farPlane, m_shaderPrograms}
{
	auto firstModelIter = m_models.begin();

	setCameraType(m_cameraType);
	zoomCamera(0.5);
	addPitchCamera(glm::radians(-30.0f));
	addYawCamera(glm::radians(15.0f));

	updateShaders();
}

void Scene::render()
{
	m_activeCamera->use({m_windowWidth, m_windowHeight});
	renderModels();
	renderCursor();
	renderActiveModelsCenter();
	renderGrid();
}

void Scene::setWindowSize(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;

	setAspectRatio(static_cast<float>(width) / height);
}

const Camera& Scene::getActiveCamera() const
{
	return *m_activeCamera;
}

Camera& Scene::getActiveCamera()
{
	return *m_activeCamera;
}

int Scene::getModelCount() const
{
	return static_cast<int>(m_models.size());
}

bool Scene::isModelVirtual(int i) const
{
	return m_models[i]->isVirtual();
}

bool Scene::isModelActive(int i) const
{
	return m_models[i]->isActive();
}

std::string Scene::getModelName(int i) const
{
	return m_models[i]->getName();
}

void Scene::updateModelGUI(int i)
{
	m_models[i]->updateGUI();
}

void Scene::setModelIsActive(int i, bool isActive)
{
	m_models[i]->setIsActive(isActive);
	
	if (isActive)
	{
		m_activeModelsCenter.addModel(m_models[i]);
	}
	else
	{
		m_activeModelsCenter.deleteModel(m_models[i]);
	}
}

Cursor& Scene::getCursor()
{
	return m_cursor;
}

CenterPoint& Scene::getActiveModelsCenter()
{
	return m_activeModelsCenter;
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

RenderMode Scene::getRenderMode() const
{
	return m_renderMode;
}

CameraType Scene::getCameraType() const
{
	return m_cameraType;
}

void Scene::setRenderMode(RenderMode renderMode)
{
	m_renderMode = renderMode;
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

void Scene::addPoint()
{
	std::unique_ptr<Point> point = std::make_unique<Point>(m_shaderPrograms.point,
		m_cursor.getPosition());

	if (m_activeModelsCenter.getModelCount() == 1)
	{
		auto activeBezierCurveC0 = std::find_if(m_bezierCurvesC0.begin(), m_bezierCurvesC0.end(),
			[] (const std::unique_ptr<BezierCurveC0>& curve)
			{
				return curve->isActive();
			}
		);

		if (activeBezierCurveC0 != m_bezierCurvesC0.end())
		{
			(*activeBezierCurveC0)->addPoints({point.get()});
		}

		auto activeBezierCurveC2 = std::find_if(m_bezierCurvesC2.begin(), m_bezierCurvesC2.end(),
			[] (const std::unique_ptr<BezierCurveC2>& curve)
			{
				return curve->isActive();
			}
		);

		if (activeBezierCurveC2 != m_bezierCurvesC2.end())
		{
			addVirtualPoints((*activeBezierCurveC2)->addPoints({point.get()}));
		}

		auto activeBezierCurveInter = std::find_if(m_bezierCurvesInter.begin(),
			m_bezierCurvesInter.end(),
			[] (const std::unique_ptr<BezierCurveInter>& curve)
			{
				return curve->isActive();
			}
		);

		if (activeBezierCurveInter != m_bezierCurvesInter.end())
		{
			(*activeBezierCurveInter)->addPoints({point.get()});
		}
	}
	
	m_models.push_back(point.get());
	m_points.push_back(std::move(point));
}

void Scene::addTorus()
{
	std::unique_ptr<Torus> torus = std::make_unique<Torus>(m_shaderPrograms.wireframe,
		m_shaderPrograms.solid, m_cursor.getPosition());
	m_models.push_back(torus.get());
	m_toruses.push_back(std::move(torus));
}

void Scene::addBezierCurveC0()
{
	std::vector<Point*> nonVirtualActivePoints = getNonVirtualActivePoints();
	if (m_activeModelsCenter.getModelCount() != nonVirtualActivePoints.size() ||
		nonVirtualActivePoints.size() == 0)
	{
		return;
	}

	std::unique_ptr<BezierCurveC0> curve = std::make_unique<BezierCurveC0>(
		m_shaderPrograms.bezierCurve, m_shaderPrograms.bezierCurvePolyline, nonVirtualActivePoints);
	m_models.push_back(curve.get());
	m_bezierCurvesC0.push_back(std::move(curve));
}

void Scene::addBezierCurveC2()
{
	std::vector<Point*> nonVirtualActivePoints = getNonVirtualActivePoints();
	if (m_activeModelsCenter.getModelCount() != nonVirtualActivePoints.size() ||
		nonVirtualActivePoints.size() == 0)
	{
		return;
	}

	auto [curve, virtualPoints] = BezierCurveC2::create(m_shaderPrograms.bezierCurve,
		m_shaderPrograms.bezierCurvePolyline, m_shaderPrograms.point, nonVirtualActivePoints);
	m_models.push_back(curve.get());
	m_bezierCurvesC2.push_back(std::move(curve));
	addVirtualPoints(std::move(virtualPoints));
}

void Scene::addBezierCurveInter()
{
	std::vector<Point*> nonVirtualActivePoints = getNonVirtualActivePoints();
	if (m_activeModelsCenter.getModelCount() != nonVirtualActivePoints.size() ||
		nonVirtualActivePoints.size() == 0)
	{
		return;
	}

	std::unique_ptr<BezierCurveInter> curve = std::make_unique<BezierCurveInter>(
		m_shaderPrograms.bezierCurve, m_shaderPrograms.bezierCurvePolyline, nonVirtualActivePoints);
	m_models.push_back(curve.get());
	m_bezierCurvesInter.push_back(std::move(curve));
}

void Scene::addActivePointsToCurve()
{
	auto activeBezierCurveC0 = std::find_if(m_bezierCurvesC0.begin(), m_bezierCurvesC0.end(),
		[] (const std::unique_ptr<BezierCurveC0>& curve)
		{
			return curve->isActive();
		}
	);
	if (activeBezierCurveC0 != m_bezierCurvesC0.end())
	{
		std::vector<Point*> nonVirtualActivePoints = getNonVirtualActivePoints();
		if (m_activeModelsCenter.getModelCount() == nonVirtualActivePoints.size() + 1 &&
			nonVirtualActivePoints.size() != 0)
		{
			(*activeBezierCurveC0)->addPoints(nonVirtualActivePoints);
		}
	}

	auto activeBezierCurveC2 = std::find_if(m_bezierCurvesC2.begin(), m_bezierCurvesC2.end(),
		[] (const std::unique_ptr<BezierCurveC2>& curve)
		{
			return curve->isActive();
		}
	);
	if (activeBezierCurveC2 != m_bezierCurvesC2.end())
	{
		std::vector<Point*> nonVirtualActivePoints = getNonVirtualActivePoints();
		if (m_activeModelsCenter.getModelCount() == nonVirtualActivePoints.size() + 1 &&
			nonVirtualActivePoints.size() != 0)
		{
			addVirtualPoints((*activeBezierCurveC2)->addPoints(nonVirtualActivePoints));
		}
	}

	auto activeBezierCurveInter = std::find_if(m_bezierCurvesInter.begin(),
		m_bezierCurvesInter.end(),
		[] (const std::unique_ptr<BezierCurveInter>& curve)
		{
			return curve->isActive();
		}
	);
	if (activeBezierCurveInter != m_bezierCurvesInter.end())
	{
		std::vector<Point*> nonVirtualActivePoints = getNonVirtualActivePoints();
		if (m_activeModelsCenter.getModelCount() == nonVirtualActivePoints.size() + 1 &&
			nonVirtualActivePoints.size() != 0)
		{
			(*activeBezierCurveInter)->addPoints(nonVirtualActivePoints);
		}
	}
}

void Scene::clearActiveModels()
{
	for (Model* model : m_models)
	{
		model->setIsActive(false);
	}
	m_activeModelsCenter.clearModels();
}

void Scene::deleteActiveModels()
{
	std::erase_if(m_models,
		[] (Model* model)
		{
			return model->isActive() && !model->isVirtual();
		}
	);
	std::erase_if(m_points,
		[] (const std::unique_ptr<Point>& point)
		{
			return point->isActive() && !point->isVirtual();
		}
	);
	std::erase_if(m_toruses,
		[] (const std::unique_ptr<Torus>& torus)
		{
			return torus->isActive() && !torus->isVirtual();
		}
	);
	std::erase_if(m_bezierCurvesC0,
		[] (const std::unique_ptr<BezierCurveC0>& curve)
		{
			return curve->isActive() && !curve->isVirtual();
		}
	);
	std::erase_if(m_bezierCurvesC2,
		[] (const std::unique_ptr<BezierCurveC2>& curve)
		{
			return curve->isActive() && !curve->isVirtual();
		}
	);
	std::erase_if(m_bezierCurvesInter,
		[] (const std::unique_ptr<BezierCurveInter>& curve)
		{
			return curve->isActive() && !curve->isVirtual();
		}
	);
	
	deleteEmptyBezierCurvesC0();
	deleteEmptyBezierCurvesC2();
	deleteEmptyBezierCurvesInter();
	deleteUnreferencedVirtualPoints();

	m_activeModelsCenter.clearModels();
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

void Scene::activate(float xPos, float yPos, bool toggle)
{
	std::optional<int> closestModel = getClosestModel(xPos, yPos);
	if (!closestModel.has_value())
	{
		return;
	}

	if (toggle)
	{
		bool wasActive = m_models[*closestModel]->isActive();
		setModelIsActive(*closestModel, !wasActive);
	}
	else
	{
		clearActiveModels();
		setModelIsActive(*closestModel, true);
		m_dragging = true;
	}
}

void Scene::release()
{
	m_dragging = false;
}

void Scene::moveActiveModel(float xPos, float yPos) const
{
	if (m_dragging)
	{
		Model* activeModel = getUniqueActiveModel();
		if (activeModel != nullptr)
		{
			activeModel->setScreenPosition({xPos, yPos}, m_activeCamera->getMatrix(),
				{m_windowWidth, m_windowHeight});
		}
	}
}

Model* Scene::getUniqueActiveModel() const
{
	std::vector<Model*> activeModels = m_activeModelsCenter.getModels();
	if (activeModels.size() == 1)
	{
		return activeModels[0];
	}
	return nullptr;
}

void Scene::setAspectRatio(float aspectRatio)
{
	m_perspectiveCamera.setAspectRatio(aspectRatio);
	m_orthographicCamera.setAspectRatio(aspectRatio);
}

void Scene::renderModels() const
{
	for (Model* model : m_models)
	{
		model->render(m_renderMode);
	}
}

void Scene::renderCursor() const
{
	m_shaderPrograms.cursor.use();
	m_cursor.render(m_shaderPrograms.cursor);
}

void Scene::renderActiveModelsCenter()
{
	m_shaderPrograms.cursor.use();
	m_activeModelsCenter.render(m_shaderPrograms.cursor);
}

void Scene::renderGrid() const
{
	m_shaderPrograms.grid.use();
	m_grid.render(m_shaderPrograms.grid, m_cameraType);
}

void Scene::updateShaders() const
{
	m_shaderPrograms.solid.use();
	m_shaderPrograms.solid.setUniform("ambient", m_ambient);
	m_shaderPrograms.solid.setUniform("diffuse", m_diffuse);
	m_shaderPrograms.solid.setUniform("specular", m_specular);
	m_shaderPrograms.solid.setUniform("shininess", m_shininess);
}

std::optional<int> Scene::getClosestModel(float xPos, float yPos) const
{
	std::optional<int> index = std::nullopt;
	constexpr float treshold = 30;
	float minScreenDistanceSquared = treshold * treshold;
	for (int i = 0; i < m_models.size(); ++i)
	{
		float screenDistanceSquared = m_models[i]->screenDistanceSquared(xPos, yPos,
			m_activeCamera->getMatrix(), {m_windowWidth, m_windowHeight});
		if (screenDistanceSquared < minScreenDistanceSquared)
		{
			index = i;
			minScreenDistanceSquared = screenDistanceSquared;
		}
	}

	return index;
}

std::vector<Point*> Scene::getNonVirtualActivePoints() const
{
	std::vector<Model*> activeModels = m_activeModelsCenter.getModels();
	std::vector<Point*> activePoints{};
	for (Model* model : activeModels)
	{
		if (model->isVirtual())
		{
			continue;
		}
		auto nonVirtualActivePoint = std::find_if(m_points.begin(), m_points.end(),
			[model] (const std::unique_ptr<Point>& point)
			{
				return model == point.get();
			}
		);
		if (nonVirtualActivePoint != m_points.end())
		{
			activePoints.push_back(nonVirtualActivePoint->get());
		}
	}
	return activePoints;
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
