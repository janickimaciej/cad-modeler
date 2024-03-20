#include "scene.hpp"

#include "models/point.hpp"
#include "models/torus.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>

constexpr float viewWidth = 20.0f;
constexpr float fovYDeg = 60.0f;
constexpr float nearPlane = 0.1f;
constexpr float farPlane = 1000.0f;

Scene::Scene(float aspectRatio) :
	m_perspectiveCamera{fovYDeg, aspectRatio, nearPlane, farPlane, m_shaderPrograms},
	m_orthographicCamera{viewWidth, aspectRatio, nearPlane, farPlane, m_shaderPrograms}
{
	auto firstModelIter = m_models.begin();
	m_activeModel = (firstModelIter != m_models.end() ? firstModelIter->get() : nullptr);

	setCameraType(m_cameraType);
	zoomCamera(0.5);
	addPitchCamera(glm::radians(-30.0f));
	addYawCamera(glm::radians(15.0f));

	updateShaders();
}

void Scene::render()
{
	m_activeCamera->use();
	renderModels();
	renderCursor();
	renderActiveModelsCenter();
	renderGrid();}

Camera& Scene::getActiveCamera()
{
	return *m_activeCamera;
}

std::vector<Model*> Scene::getModels()
{
	std::vector<Model*> models{};
	for (const std::unique_ptr<Model>& model : m_models)
	{
		models.push_back(model.get());
	}
	return models;
}

void Scene::updateModelGUI(int i)
{
	m_models[i]->getGUI().update();
}

void Scene::setModelIsActive(int i, bool isActive)
{
	m_models[i]->setIsActive(isActive);

	std::vector<Model*> activeModels{};
	for (const std::unique_ptr<Model>& model : m_models)
	{
		if (model->isActive())
		{
			activeModels.push_back(model.get());
		}
	}
	m_activeModelsCenter.setModels(activeModels);
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

void Scene::setAspectRatio(float aspectRatio)
{
	m_perspectiveCamera.setAspectRatio(aspectRatio);
	m_orthographicCamera.setAspectRatio(aspectRatio);
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
	m_models.push_back(std::make_unique<Point>(m_shaderPrograms.wireframePoint,
		m_shaderPrograms.solidPoint, m_cursor.getPosition()));
}

void Scene::addTorus()
{
	m_models.push_back(std::make_unique<Torus>(m_shaderPrograms.wireframe,
		m_shaderPrograms.solid, m_cursor.getPosition()));
}

void Scene::clearActiveModels()
{
	for (std::unique_ptr<Model>& model : m_models)
	{
		model->setIsActive(false);
	}
	m_activeModelsCenter.setModels(std::vector<Model*>{});
}

void Scene::deleteActiveModels()
{
	std::erase_if(m_models,
		[] (const std::unique_ptr<Model>& model)
		{
			return model->isActive();
		}
	);
}

Model* Scene::getUniqueActiveModel() const
{
	Model* uniqueActiveModel = nullptr;

	for (const std::unique_ptr<Model>& model : m_models)
	{
		if (model->isActive())
		{
			if (uniqueActiveModel != nullptr)
			{
				return nullptr;
			}

			uniqueActiveModel = model.get();
		}
	}

	return uniqueActiveModel;
}

void Scene::renderModels() const
{
	for (const std::unique_ptr<Model>& model : m_models)
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
