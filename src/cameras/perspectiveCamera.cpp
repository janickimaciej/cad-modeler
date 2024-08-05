#include "cameras/perspectiveCamera.hpp"

#include <cmath>

PerspectiveCamera::PerspectiveCamera(float fovYDeg, float aspectRatio, float nearPlane,
	float farPlane, const ShaderPrograms& shaderPrograms) :
	Camera{aspectRatio, nearPlane, farPlane, shaderPrograms},
	m_gui{*this},
	m_fovYDeg{fovYDeg}
{
	updateProjectionMatrix();
}

CameraGUI& PerspectiveCamera::getGUI()
{
	return m_gui;
}

float PerspectiveCamera::getFOVYDeg() const
{
	return m_fovYDeg;
}

void PerspectiveCamera::setFOVYDeg(float fovYDeg)
{
	m_fovYDeg = fovYDeg;
	updateProjectionMatrix();
}

void PerspectiveCamera::zoom(float zoom)
{
	m_radius /= zoom;
	updateViewMatrix();
}

void PerspectiveCamera::updateProjectionMatrix()
{
	float fovYRad = glm::radians(m_fovYDeg);
	float cot = std::cos(fovYRad / 2) / std::sin(fovYRad / 2);
	m_projectionMatrix =
	{
		cot / m_aspectRatio, 0, 0, 0,
		0, cot, 0, 0,
		0, 0, -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane), -1,
		0, 0, -2 * m_farPlane * m_nearPlane / (m_farPlane - m_nearPlane), 0
	};
}
