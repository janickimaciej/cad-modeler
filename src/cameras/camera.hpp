#pragma once

#include "anaglyphMode.hpp"

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(const glm::ivec2& viewportSize, float nearPlane, float farPlane);
	virtual ~Camera() = default;

	void use() const;
	void useLeftEye() const;
	void useRightEye() const;
	virtual void updateGUI() = 0;
	glm::mat4 getMatrix() const;
	void updateViewportSize();

	void setTargetPos(const glm::vec3& pos);
	void moveX(float x);
	void moveY(float y);
	void addPitch(float pitchRad);
	void addYaw(float yawRad);
	virtual void zoom(float zoom) = 0;

	glm::vec2 posToViewportPos(const glm::vec3& pos) const;
	glm::vec3 viewportPosToPos(const glm::vec3& prevPos, const glm::vec2& viewportPos) const;

	float getEyesDistance() const;
	void setEyesDistance(float eyesDistance);
	float getScreenDistance() const;
	void setScreenDistance(float screenDistance);
	float getProjectionPlane() const;
	void setProjectionPlane(float projectionPlane);

protected:
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

	float getAspectRatio() const;

private:
	const glm::ivec2& m_viewportSize{};
	glm::vec3 m_targetPos{0, 0, 0};
	float m_pitchRad = 0;
	float m_yawRad = 0;

	glm::vec3 getPos() const;
	void updateShaders(const glm::mat4& projectionViewMatrix,
		AnaglyphMode anaglyphMode) const;
};
