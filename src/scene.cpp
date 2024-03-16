#include "scene.hpp"

#include "models/torus.hpp"
#include "window.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>

constexpr float viewWidth = 20.0f;
constexpr float fovYDeg = 60.0f;
constexpr float nearPlane = 0.1f;
constexpr float farPlane = 1000.0f;

Scene::Scene(float aspectRatio, Window& window) :
	m_perspectiveCamera{fovYDeg, aspectRatio, nearPlane, farPlane, m_gridShaderProgram,
		m_wireframeShaderProgram, m_solidShaderProgram},
	m_orthographicCamera{viewWidth, aspectRatio, nearPlane, farPlane, m_gridShaderProgram,
		m_wireframeShaderProgram, m_solidShaderProgram}
{
	m_models.push_back(std::make_unique<Torus>(3.0f, 1.0f, 32, 16));
	m_activeModel = m_models.back().get();
	window.setScene(*this);

	setCameraType(m_cameraType);
	zoom(0.5);
	addPitch(glm::radians(-30.0f));
	addYaw(glm::radians(30.0f));

	updateShaders();
}

void Scene::render() const
{
	m_activeCamera->use();
	renderModels();
	renderGrid();
}

Camera& Scene::getActiveCamera()
{
	return *m_activeCamera;
}

Model& Scene::getActiveModel()
{
	return *m_activeModel;
}

void Scene::addPitch(float pitchRad)
{
	m_perspectiveCamera.addPitch(pitchRad);
	m_orthographicCamera.addPitch(pitchRad);
}

void Scene::addYaw(float yawRad)
{
	m_perspectiveCamera.addYaw(yawRad);
	m_orthographicCamera.addYaw(yawRad);
}

void Scene::addRadius(float radius)
{
	m_perspectiveCamera.addRadius(radius);
	m_orthographicCamera.addRadius(radius);
}

void Scene::moveX(float x)
{
	m_perspectiveCamera.moveX(x);
	m_orthographicCamera.moveX(x);
}

void Scene::moveY(float y)
{
	m_perspectiveCamera.moveY(y);
	m_orthographicCamera.moveY(y);
}

void Scene::zoom(float zoom)
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

void Scene::renderModels() const
{
	const ShaderProgram* activeShaderProgram = &m_solidShaderProgram;
	switch (m_renderMode)
	{
	case RenderMode::wireframe:
		activeShaderProgram = &m_wireframeShaderProgram;
		break;

	case RenderMode::solid:
		activeShaderProgram = &m_solidShaderProgram;
		break;
	}
	activeShaderProgram->use();
	for (const std::unique_ptr<Model>& model : m_models)
	{
		model->render(m_renderMode, *activeShaderProgram);
	}
}

void Scene::renderGrid() const
{
	m_gridShaderProgram.use();
	m_grid.render(m_gridShaderProgram, m_cameraType);
}

void Scene::updateShaders() const
{
	m_solidShaderProgram.use();
	m_solidShaderProgram.setUniform1f("ambient", m_ambient);
	m_solidShaderProgram.setUniform1f("diffuse", m_diffuse);
	m_solidShaderProgram.setUniform1f("specular", m_specular);
	m_solidShaderProgram.setUniform1f("shininess", m_shininess);
}
