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

void Scene::update()
{
	std::vector<BezierCurve*> bezierCurvesToBeDeleted{};
	std::erase_if(m_bezierCurves,
		[&bezierCurvesToBeDeleted] (const std::unique_ptr<BezierCurve>& curve)
		{
			if (curve->getPointCount() == 0)
			{
				bezierCurvesToBeDeleted.push_back(curve.get());
				return true;
			}
			return false;
		}
	);
	for (BezierCurve* curve : bezierCurvesToBeDeleted)
	{
		std::erase_if(m_models,
			[&curve] (Model* model)
			{
				return model == curve;
			}
		);
		m_activeModelsCenter.deleteModel(curve);
	}
}

void Scene::render()
{
	m_activeCamera->use(glm::vec2{m_windowWidth, m_windowHeight});
	renderModels();
	renderCursor();
	renderActiveModelsCenter();
	renderGrid();
}

glm::ivec2 Scene::getWindowSize() const
{
	return glm::ivec2{m_windowWidth, m_windowHeight};
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

std::vector<Model*> Scene::getModels()
{
	return m_models;
}

void Scene::updateModelGUI(int i)
{
	m_models[i]->getGUI().update();
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
	std::unique_ptr<Point> point = std::make_unique<Point>(*this, m_shaderPrograms.wireframePoint,
		m_shaderPrograms.solidPoint, m_cursor.getPosition());
	m_models.push_back(point.get());

	if (m_activeModelsCenter.getModelCount() == 1)
	{
		auto activeBezierCurve = std::find_if(m_bezierCurves.begin(), m_bezierCurves.end(),
			[] (const std::unique_ptr<BezierCurve>& curve)
			{
				return curve->isActive();
			}
		);

		if (activeBezierCurve != m_bezierCurves.end())
		{
			(*activeBezierCurve)->addPoints({point.get()});
		}
	}

	m_points.push_back(std::move(point));
}

void Scene::addTorus()
{
	std::unique_ptr<Torus> torus = std::make_unique<Torus>(*this, m_shaderPrograms.wireframe,
		m_shaderPrograms.solid, m_cursor.getPosition());
	m_models.push_back(torus.get());
	m_toruses.push_back(std::move(torus));
}

void Scene::addBezierCurve()
{
	std::vector<Point*> activePoints = getActivePoints();
	if (m_activeModelsCenter.getModelCount() != activePoints.size() || activePoints.size() == 0)
	{
		return;
	}

	std::unique_ptr<BezierCurve> bezierCurve = std::make_unique<BezierCurve>(*this,
		m_shaderPrograms.bezierCurve, m_shaderPrograms.bezierCurvePolyline, activePoints);
	m_models.push_back(bezierCurve.get());
	m_bezierCurves.push_back(std::move(bezierCurve));
}

void Scene::addActivePointsToCurve()
{
	auto activeBezierCurve = std::find_if(m_bezierCurves.begin(), m_bezierCurves.end(),
		[] (const std::unique_ptr<BezierCurve>& curve)
		{
			return curve->isActive();
		}
	);
	if (activeBezierCurve == m_bezierCurves.end())
	{
		return;
	}
	
	std::vector<Point*> activePoints = getActivePoints();
	if (m_activeModelsCenter.getModelCount() != activePoints.size() + 1 || activePoints.size() == 0)
	{
		return;
	}

	(*activeBezierCurve)->addPoints(activePoints);
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
			return model->isActive();
		}
	);
	std::erase_if(m_points,
		[] (const std::unique_ptr<Point>& point)
		{
			return point->isActive();
		}
	);
	std::erase_if(m_toruses,
		[] (const std::unique_ptr<Torus>& torus)
		{
			return torus->isActive();
		}
	);
	
	std::vector<BezierCurve*> bezierCurvesToBeDeleted{};
	std::erase_if(m_bezierCurves,
		[&bezierCurvesToBeDeleted] (const std::unique_ptr<BezierCurve>& curve)
		{
			if (curve->isActive())
			{
				bezierCurvesToBeDeleted.push_back(curve.get());
				return true;
			}
			return false;
		}
	);
	for (BezierCurve* curve : bezierCurvesToBeDeleted)
	{
		std::erase_if(m_models,
			[&curve] (Model* model)
			{
				return model == curve;
			}
		);
	}

	m_activeModelsCenter.clearModels();
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
			activeModel->setScreenPosition(glm::ivec2{xPos, yPos});
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
	m_shaderPrograms.solid.setUniform1f("ambient", m_ambient);
	m_shaderPrograms.solid.setUniform1f("diffuse", m_diffuse);
	m_shaderPrograms.solid.setUniform1f("specular", m_specular);
	m_shaderPrograms.solid.setUniform1f("shininess", m_shininess);
}

std::optional<int> Scene::getClosestModel(float xPos, float yPos) const
{
	std::optional<int> index = std::nullopt;
	constexpr float treshold = 30;
	float minDistanceSquared = treshold * treshold;
	for (int i = 0; i < m_models.size(); ++i)
	{
		float distanceSquared =
			m_models[i]->distanceSquared(xPos, yPos, m_windowWidth, m_windowHeight,
				m_activeCamera->getMatrix());
		if (distanceSquared < minDistanceSquared)
		{
			index = i;
			minDistanceSquared = distanceSquared;
		}
	}

	return index;
}

std::vector<Point*> Scene::getActivePoints() const
{
	std::vector<Model*> activeModels = m_activeModelsCenter.getModels();
	std::vector<Point*> activePoints{};
	for (Model* model : activeModels)
	{
		auto activePoint = std::find_if(m_points.begin(), m_points.end(),
			[model] (const std::unique_ptr<Point>& point)
			{
				return model == point.get();
			}
		);
		if (activePoint != m_points.end())
		{
			activePoints.push_back(activePoint->get());
		}
	}
	return activePoints;
}
