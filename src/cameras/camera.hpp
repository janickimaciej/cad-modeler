#pragma once

#include <glm/glm.hpp>

#include "guis/camera_guis/camera_gui.hpp"
#include "shader_programs.hpp"

class Camera
{
public:
	Camera(float aspectRatio, float nearPlane, float farPlane,
		const ShaderPrograms& shaderPrograms);
	void use(const glm::vec2& windowSize) const;
	virtual CameraGUI& getGUI() = 0;
	glm::vec3 getPosition() const;
	glm::mat4 getMatrix() const;
	void addPitch(float pitchRad);
	void addYaw(float yawRad);
	void addRadius(float radius);
	void moveX(float x);
	void moveY(float y);
	virtual void zoom(float zoom) = 0;
	void setAspectRatio(float aspectRatio);

protected:
	float m_aspectRatio{};
	float m_nearPlane{};
	float m_farPlane{};
	float m_radius = 10;

	glm::mat4 m_viewMatrixInverse{1};
	glm::mat4 m_projectionMatrix{1};
	
	void updateViewMatrix();
	virtual void updateProjectionMatrix() = 0;
	void updateShaders(const glm::vec2& windowSize) const;

private:
	glm::vec3 m_targetPosition{0, 0, 0};
	float m_pitchRad = 0;
	float m_yawRad = 0;
	
	const ShaderPrograms& m_shaderPrograms;
};
