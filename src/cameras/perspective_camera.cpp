#include "cameras/perspective_camera.hpp"

#include <cmath>

PerspectiveCamera::PerspectiveCamera(float fovRad, float aspectRatio, float nearPlane,
	float farPlane, ShaderProgram& solidShaderProgram, ShaderProgram& wireframeShaderProgram) :
	Camera{aspectRatio, nearPlane, farPlane, solidShaderProgram, wireframeShaderProgram},
	m_gui{*this},
	m_fovRad{fovRad}
{
	updateProjectionMatrix();
}

CameraGUI& PerspectiveCamera::getGUI()
{
	return m_gui;
}

float PerspectiveCamera::getFOVRad() const
{
	return m_fovRad;
}

void PerspectiveCamera::setFOVRad(float FOVRad)
{
	m_fovRad = FOVRad;

	updateProjectionMatrix();
}

void PerspectiveCamera::zoom(float zoom)
{
	m_radius /= zoom;

	updateViewMatrix();
}

void PerspectiveCamera::updateProjectionMatrix()
{
	float cot = std::cos(m_fovRad / 2) / std::sin(m_fovRad / 2);
	m_projectionMatrix =
		glm::mat4
		{
			cot / m_aspectRatio, 0, 0, 0,
			0, cot, 0, 0,
			0, 0, -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane), -1,
			0, 0, -2 * m_farPlane * m_nearPlane / (m_farPlane - m_nearPlane), 0
		};
}
