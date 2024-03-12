#include "scene.hpp"

#include "models/torus.hpp"
#include "window.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>

constexpr float viewWidth = 20.0f;
constexpr float fovDeg = 60.0f;
constexpr float nearPlane = 0.1f;
constexpr float farPlane = 100.0f;

Scene::Scene(float aspectRatio, Window& window) :
	m_orthographicCamera{viewWidth, aspectRatio, nearPlane, farPlane, m_solidShaderProgram,
		m_wireframeShaderProgram},
	m_perspectiveCamera{fovDeg, aspectRatio, nearPlane, farPlane, m_solidShaderProgram,
		m_wireframeShaderProgram}
{
	m_models.push_back(std::make_unique<Torus>(3.0f, 0.3f, 16, 8));
	m_activeModel = m_models.back().get();
	window.setScene(*this);

	switch (m_cameraType)
	{
	case CameraType::orthographic:
		m_activeCamera = &m_orthographicCamera;
		break;

	case CameraType::perspective:
		m_activeCamera = &m_perspectiveCamera;
		break;
	}
}

void Scene::render()
{
	constexpr glm::vec3 backgroundColor{0.1f, 0.1f, 0.1f};
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderProgram* activeShaderProgram = &m_solidShaderProgram;
	switch (m_renderMode)
	{
	case RenderMode::solid:
		activeShaderProgram = &m_solidShaderProgram;
		break;
		
	case RenderMode::wireframe:
		activeShaderProgram = &m_wireframeShaderProgram;
		break;
	}

	m_activeCamera->use();
	activeShaderProgram->use();
	for (const std::unique_ptr<Model>& model : m_models)
	{
		model->render(*activeShaderProgram);
	}
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
	m_orthographicCamera.addPitch(pitchRad);
	m_perspectiveCamera.addPitch(pitchRad);
}

void Scene::addYaw(float yawRad)
{
	m_orthographicCamera.addYaw(yawRad);
	m_perspectiveCamera.addYaw(yawRad);
}

void Scene::addRadius(float radius)
{
	m_orthographicCamera.addRadius(radius);
	m_perspectiveCamera.addRadius(radius);
}

void Scene::moveX(float x)
{
	m_orthographicCamera.moveX(x);
	m_perspectiveCamera.moveX(x);
}

void Scene::moveY(float y)
{
	m_orthographicCamera.moveY(y);
	m_perspectiveCamera.moveY(y);
}

void Scene::zoom(float zoom)
{
	m_orthographicCamera.zoom(zoom);
	m_perspectiveCamera.zoom(zoom);
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
	m_orthographicCamera.setAspectRatio(aspectRatio);
	m_perspectiveCamera.setAspectRatio(aspectRatio);
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
	case CameraType::orthographic:
		m_activeCamera = &m_orthographicCamera;
		break;
	case CameraType::perspective:
		m_activeCamera = &m_perspectiveCamera;
		break;
	}
}
