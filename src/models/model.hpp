#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "renderMode.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <string>

class Model
{
public:
	Model(const glm::vec3& position, const std::string& name, bool isVirtual = false);
	~Model() = default;

	virtual void render(RenderMode renderMode) const = 0;
	virtual void updateGUI() = 0;

	glm::vec3 getPosition() const;
	virtual void setPosition(const glm::vec3& position);
	glm::vec2 getScreenPosition(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize) const;
	virtual void setScreenPosition(const glm::vec2& screenPosition, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize);

	float getYawRad() const;
	void setYawRad(float yawRad);
	float getPitchRad() const;
	void setPitchRad(float pitchRad);
	float getRollRad() const;
	void setRollRad(float rollRad);

	glm::vec3 getScale() const;
	void setScale(const glm::vec3& scale);

	std::string getOriginalName() const;
	std::string getName() const;
	void setName(const std::string& name);
	
	bool isVirtual() const;
	bool isActive() const;
	void setIsActive(bool isActive);

	float screenDistanceSquared(float xPos, float yPos, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize) const;

	glm::mat3 getRotationMatrix() const;

protected:
	glm::mat4 m_modelMatrix{1};

	glm::vec3 m_position{};
	float m_yawRad = 0;
	float m_pitchRad = 0;
	float m_rollRad = 0;
	glm::vec3 m_scale = {1, 1, 1};

	void updateMatrix();
	virtual void updateShaders(RenderMode renderMode) const = 0;

private:
	const std::string m_originalName{};
	std::string m_name{};

	bool m_isActive = false;
	bool m_isVirtual{};
};
