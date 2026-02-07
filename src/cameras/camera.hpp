#pragma once

#include "anaglyphMode.hpp"

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(const glm::ivec2& windowSize, float nearPlane, float farPlane);
	virtual ~Camera() = default;
	void use() const;
	void useLeftEye() const;
	void useRightEye() const;
	virtual void updateGUI() = 0;
	glm::mat4 getMatrix() const;
	void updateWindowSize();

	void addPitch(float pitchRad);
	void addYaw(float yawRad);
	void setTargetPos(const glm::vec3& pos);
	void moveX(float x);
	void moveY(float y);
	virtual void zoom(float zoom) = 0;

	glm::vec2 posToScreenPos(const glm::vec3& pos) const;
	glm::vec3 screenPosToPos(const glm::vec3& prevPos, const glm::vec2& screenPos) const;

	float getEyesDistance() const;
	void setEyesDistance(float eyesDistance);
	float getScreenDistance() const;
	void setScreenDistance(float screenDistance);
	float getProjectionPlane() const;
	void setProjectionPlane(float projectionPlane);

protected:
	const glm::ivec2& m_windowSize{};
	float m_nearPlane{};
	float m_farPlane{};
	float m_radius = 3;

	glm::mat4 m_viewMatrixInverse{1};
	glm::mat4 m_leftEyeViewMatrixInverse{1};
	glm::mat4 m_rightEyeViewMatrixInverse{1};

	glm::mat4 m_projectionMatrix{1};
	glm::mat4 m_leftEyeProjectionMatrix{1};
	glm::mat4 m_rightEyeProjectionMatrix{1};

	float m_eyesDistance = 60;
	float m_screenDistance = 1000;
	float m_projectionPlane = 1;

	void updateViewMatrix();
	virtual void updateProjectionMatrix() = 0;
	void updateShaders() const;
	void updateShadersLeftEye() const;
	void updateShadersRightEye() const;

private:
	glm::vec3 m_targetPos{0, 0, 0};
	float m_pitchRad = 0;
	float m_yawRad = 0;

	void updateShaders(const glm::mat4& projectionViewMatrix,
		AnaglyphMode anaglyphMode) const;
	glm::vec3 getPos() const;
};
