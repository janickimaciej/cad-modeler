#include "cameras/camera.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>

Camera::Camera(const glm::ivec2& windowSize, float nearPlane, float farPlane,
	const ShaderPrograms& shaderPrograms) :
	m_windowSize{windowSize},
	m_nearPlane{nearPlane},
	m_farPlane{farPlane},
	m_shaderPrograms{shaderPrograms}
{
	updateViewMatrix();
}

void Camera::use() const
{
	updateShaders();
}

void Camera::useLeftEye() const
{
	updateShadersLeftEye();
}

void Camera::useRightEye() const
{
	updateShadersRightEye();
}

glm::mat4 Camera::getMatrix() const
{
	return m_projectionMatrix * glm::inverse(m_viewMatrixInverse);
}

void Camera::updateWindowSize()
{
	updateProjectionMatrix();
}

void Camera::addPitch(float pitchRad)
{
	m_pitchRad += pitchRad;

	static constexpr float bound = glm::radians(89.0f);
	if (m_pitchRad < -bound)
	{
		m_pitchRad = -bound;
	}
	if (m_pitchRad > bound)
	{
		m_pitchRad = bound;
	}

	updateViewMatrix();
}

void Camera::addYaw(float yawRad)
{
	m_yawRad += yawRad;

	static constexpr float pi = glm::pi<float>();
	while (m_yawRad < -pi)
	{
		m_yawRad += 2 * pi;
	}
	while (m_yawRad >= pi)
	{
		m_yawRad -= 2 * pi;
	}

	updateViewMatrix();
}

void Camera::setTargetPos(const glm::vec3& pos)
{
	m_targetPos = pos;

	updateViewMatrix();
}

void Camera::moveX(float x)
{
	m_targetPos += m_radius * glm::mat3{m_viewMatrixInverse} * glm::vec3{x, 0, 0};

	updateViewMatrix();
}

void Camera::moveY(float y)
{
	m_targetPos += m_radius * glm::mat3{m_viewMatrixInverse} * glm::vec3{0, y, 0};

	updateViewMatrix();
}

glm::vec2 Camera::posToScreenPos(const glm::vec3& pos) const
{
	glm::vec4 clipPos = getMatrix() * glm::vec4{pos, 1};
	clipPos /= clipPos.w;
	return glm::vec2
	{
		(clipPos.x + 1) / 2 * m_windowSize.x,
		(-clipPos.y + 1) / 2 * m_windowSize.y
	};
}

glm::vec3 Camera::screenPosToPos(const glm::vec3& prevPos, const glm::vec2& screenPos) const
{
	glm::mat4 cameraMatrix = getMatrix();
	glm::vec4 prevClipPos = cameraMatrix * glm::vec4{prevPos, 1};
	prevClipPos /= prevClipPos.w;
	glm::vec4 clipPos
	{
		screenPos.x / m_windowSize.x * 2 - 1,
		-screenPos.y / m_windowSize.y * 2 + 1,
		prevClipPos.z,
		1
	};
	glm::vec4 worldPos = glm::inverse(cameraMatrix) * clipPos;
	worldPos /= worldPos.w;
	return glm::vec3{worldPos};
}

float Camera::getEyesDistance() const
{
	return m_eyesDistance;
}

void Camera::setEyesDistance(float eyesDistance)
{
	m_eyesDistance = eyesDistance;

	updateViewMatrix();
	updateProjectionMatrix();
}

float Camera::getScreenDistance() const
{
	return m_screenDistance;
}

void Camera::setScreenDistance(float screenDistance)
{
	m_screenDistance = screenDistance;

	updateViewMatrix();
	updateProjectionMatrix();
}

float Camera::getProjectionPlane() const
{
	return m_projectionPlane;
}

void Camera::setProjectionPlane(float projectionPlane)
{
	m_projectionPlane = projectionPlane;

	updateViewMatrix();
	updateProjectionMatrix();
}

void Camera::updateViewMatrix()
{
	glm::vec3 pos = m_targetPos + m_radius *
		glm::vec3
		{
			-std::cos(m_pitchRad) * std::sin(m_yawRad),
			-std::sin(m_pitchRad),
			std::cos(m_pitchRad) * std::cos(m_yawRad)
		};

	glm::vec3 direction = glm::normalize(pos - m_targetPos);
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3{0, 1, 0}, direction));
	glm::vec3 up = glm::cross(direction, right);

	float virtualEyesDistance = m_projectionPlane / m_screenDistance * m_eyesDistance;
	glm::vec3 leftEyePos = pos - right * virtualEyesDistance / 2.0f;
	glm::vec3 rightEyePos = pos + right * virtualEyesDistance / 2.0f;

	m_viewMatrixInverse =
	{
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		direction.x, direction.y, direction.z, 0,
		pos.x, pos.y, pos.z, 1
	};

	m_leftEyeViewMatrixInverse =
	{
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		direction.x, direction.y, direction.z, 0,
		leftEyePos.x, leftEyePos.y, leftEyePos.z, 1
	};

	m_rightEyeViewMatrixInverse =
	{
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		direction.x, direction.y, direction.z, 0,
		rightEyePos.x, rightEyePos.y, rightEyePos.z, 1
	};
}

void Camera::updateShaders() const
{
	glm::mat4 projectionViewMatrix = m_projectionMatrix * glm::inverse(m_viewMatrixInverse);
	updateShaders(projectionViewMatrix, AnaglyphMode::none);
}

void Camera::updateShadersLeftEye() const
{
	glm::mat4 projectionViewMatrix =
		m_leftEyeProjectionMatrix * glm::inverse(m_leftEyeViewMatrixInverse);
	updateShaders(projectionViewMatrix, AnaglyphMode::leftEye);
}

void Camera::updateShadersRightEye() const
{
	glm::mat4 projectionViewMatrix =
		m_rightEyeProjectionMatrix * glm::inverse(m_rightEyeViewMatrixInverse);
	updateShaders(projectionViewMatrix, AnaglyphMode::rightEye);
}

void Camera::updateShaders(const glm::mat4& projectionViewMatrix, AnaglyphMode anaglyphMode) const
{
	glm::mat4 projectionViewMatrixInverse = glm::inverse(projectionViewMatrix);

	m_shaderPrograms.mesh.use();
	m_shaderPrograms.mesh.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.mesh.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.point.use();
	m_shaderPrograms.point.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.point.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.cursor.use();
	m_shaderPrograms.cursor.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.cursor.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.plane.use();
	m_shaderPrograms.plane.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.plane.setUniform("projectionViewMatrixInverse", projectionViewMatrixInverse);
	m_shaderPrograms.plane.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.torus.use();
	m_shaderPrograms.torus.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.torus.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.bezierCurve.use();
	m_shaderPrograms.bezierCurve.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.bezierCurve.setUniform("windowSize", m_windowSize);
	m_shaderPrograms.bezierCurve.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.interpolatingBezierCurve.use();
	m_shaderPrograms.interpolatingBezierCurve.setUniform("projectionViewMatrix",
		projectionViewMatrix);
	m_shaderPrograms.interpolatingBezierCurve.setUniform("windowSize", m_windowSize);
	m_shaderPrograms.interpolatingBezierCurve.setUniform("anaglyphMode",
		static_cast<int>(anaglyphMode));

	m_shaderPrograms.polyline.use();
	m_shaderPrograms.polyline.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.polyline.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.bezierSurface.use();
	m_shaderPrograms.bezierSurface.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.bezierSurface.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.gregorySurface.use();
	m_shaderPrograms.gregorySurface.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.gregorySurface.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));

	m_shaderPrograms.vectors.use();
	m_shaderPrograms.vectors.setUniform("projectionViewMatrix", projectionViewMatrix);
	m_shaderPrograms.vectors.setUniform("anaglyphMode", static_cast<int>(anaglyphMode));
}
