#include "cameras/orthographic_camera.hpp"

OrthographicCamera::OrthographicCamera(float viewWidth, float aspectRatio, float nearPlane,
	float farPlane, ShaderProgram& gridShaderProgram, ShaderProgram& wireframeShaderProgram,
	ShaderProgram& solidShaderProgram) :
	Camera{aspectRatio, nearPlane, farPlane, gridShaderProgram, wireframeShaderProgram,
		solidShaderProgram},
	m_gui{*this},
	m_viewWidth{viewWidth}
{
	updateProjectionMatrix();
}

CameraGUI& OrthographicCamera::getGUI()
{
	return m_gui;
}

float OrthographicCamera::getViewWidth() const
{
	return m_viewWidth;
}

void OrthographicCamera::setViewWidth(float viewWidth)
{
	m_viewWidth = viewWidth;

	updateProjectionMatrix();
}

void OrthographicCamera::zoom(float zoom)
{
	m_viewWidth /= zoom;
	m_radius /= zoom;

	updateViewMatrix();
	updateProjectionMatrix();
}

void OrthographicCamera::updateProjectionMatrix()
{
	float viewHeight = m_viewWidth / m_aspectRatio;
	m_projectionMatrix =
		glm::mat4
		{
			2 / m_viewWidth, 0, 0, 0,
			0, 2 / viewHeight, 0, 0,
			0, 0, -2 / (m_farPlane - m_nearPlane), 0,
			0, 0, -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane), 1
		};
}
