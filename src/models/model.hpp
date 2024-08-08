#pragma once

#include <glm/glm.hpp>

#include <string>

class Model
{
public:
	Model(const glm::vec3& pos, const std::string& name, bool isVirtual = false);
	~Model() = default;

	virtual void render() const = 0;
	virtual void updateGUI() = 0;

	glm::vec3 getPos() const;
	virtual void setPos(const glm::vec3& pos);
	glm::vec2 getScreenPos(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize) const;
	virtual void setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize);
	
	float getPitchRad() const;
	void setPitchRad(float pitchRad);
	float getYawRad() const;
	void setYawRad(float yawRad);
	float getRollRad() const;
	void setRollRad(float rollRad);

	glm::vec3 getScale() const;
	void setScale(const glm::vec3& scale);

	std::string getOriginalName() const;
	std::string getName() const;
	void setName(const std::string& name);
	
	bool isVirtual() const;
	bool isSelected() const;
	void select();
	void deselect();

	glm::mat4 getRotationMatrix() const;

protected:
	glm::mat4 m_modelMatrix{};

	glm::vec3 m_pos{};
	float m_pitchRad = 0;
	float m_yawRad = 0;
	float m_rollRad = 0;
	glm::vec3 m_scale = {1, 1, 1};

	void updateMatrix();
	virtual void updateShaders() const = 0;

private:
	const std::string m_originalName{};
	std::string m_name{};

	bool m_isSelected = false;
	bool m_isVirtual{};
};
