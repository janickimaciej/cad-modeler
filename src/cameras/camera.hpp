#pragma once

#include <glm/glm.hpp>

#include "anaglyphMode.hpp"
#include "guis/cameraGUIs/cameraGUI.hpp"
#include "shaderPrograms.hpp"

class Camera
{
public:
	Camera(float aspectRatio, float nearPlane, float farPlane,
		const ShaderPrograms& shaderPrograms);
	void use(const glm::ivec2& windowSize) const;
	void useLeftEye(const glm::ivec2& windowSize) const;
	void useRightEye(const glm::ivec2& windowSize) const;
	virtual void updateGUI() = 0;
	glm::mat4 getMatrix() const;
	void setAspectRatio(float aspectRatio);

	void addPitch(float pitchRad);
	void addYaw(float yawRad);
	void moveX(float x);
	void moveY(float y);
	virtual void zoom(float zoom) = 0;

	float getEyesDistance() const;
	void setEyesDistance(float eyesDistance);
	float getScreenDistance() const;
	void setScreenDistance(float screenDistance);
	float getProjectionPlane() const;
	void setProjectionPlane(float projectionPlane);

protected:
	float m_aspectRatio{};
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
	void updateShaders(const glm::ivec2& windowSize) const;
	void updateShadersLeftEye(const glm::ivec2& windowSize) const;
	void updateShadersRightEye(const glm::ivec2& windowSize) const;

private:
	glm::vec3 m_targetPos{0, 0, 0};
	float m_pitchRad = 0;
	float m_yawRad = 0;
	
	const ShaderPrograms& m_shaderPrograms;

	void updateShaders(const glm::ivec2& windowSize, const glm::mat4& projectionViewMatrix,
		AnaglyphMode anaglyphMode) const;
};
