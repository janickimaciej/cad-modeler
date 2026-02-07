#include "cameras/orthographicCamera.hpp"

OrthographicCamera::OrthographicCamera(const glm::ivec2& viewportSize, float viewHeight,
	float nearPlane, float farPlane) :
	Camera{viewportSize, nearPlane, farPlane},
	m_gui{*this},
	m_viewHeight{viewHeight}
{
	updateProjectionMatrix();
}

void OrthographicCamera::updateGUI()
{
	m_gui.update();
}

float OrthographicCamera::getViewHeight() const
{
	return m_viewHeight;
}

void OrthographicCamera::setViewHeight(float viewHeight)
{
	m_viewHeight = viewHeight;
	updateProjectionMatrix();
}

void OrthographicCamera::zoom(float zoom)
{
	m_radius /= zoom;
	updateViewMatrix();

	m_viewHeight /= zoom;
	updateProjectionMatrix();
}

void OrthographicCamera::updateProjectionMatrix()
{
	float aspectRatio = static_cast<float>(m_viewportSize.x) / m_viewportSize.y;
	float viewWidth = m_viewHeight * aspectRatio;
	m_projectionMatrix =
	{
		2 / viewWidth, 0, 0, 0,
		0, 2 / m_viewHeight, 0, 0,
		0, 0, -2 / (m_farPlane - m_nearPlane), 0,
		0, 0, -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane), 1
	};
}
