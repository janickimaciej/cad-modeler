#include "cameras/perspective_camera.hpp"

#include <cmath>

PerspectiveCamera::PerspectiveCamera(float fovDeg, float aspectRatio, float nearPlane,
	float farPlane, ShaderProgram& solidShaderProgram, ShaderProgram& wireframeShaderProgram) :
	Camera{aspectRatio, nearPlane, farPlane, solidShaderProgram, wireframeShaderProgram},
	m_gui{*this},
	m_fovDeg{fovDeg}
{
	updateProjectionMatrix();
}

CameraGUI& PerspectiveCamera::getGUI()
{
	return m_gui;
}

float PerspectiveCamera::getFOVDeg() const
{
	return m_fovDeg;
}

void PerspectiveCamera::setFOVDeg(float FOVDeg)
{
	m_fovDeg = FOVDeg;

	updateProjectionMatrix();
}

void PerspectiveCamera::zoom(float zoom)
{
	m_radius /= zoom;

	updateViewMatrix();
}

void PerspectiveCamera::updateProjectionMatrix()
{
	float fovRad = glm::radians(m_fovDeg);
	float cot = std::cos(fovRad / 2) / std::sin(fovRad / 2);
	m_projectionMatrix =
		glm::mat4
		{
			cot / m_aspectRatio, 0, 0, 0,
			0, cot, 0, 0,
			0, 0, -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane), -1,
			0, 0, -2 * m_farPlane * m_nearPlane / (m_farPlane - m_nearPlane), 0
		};
}
