#pragma once

#include <glm/glm.hpp>

#include <string>

class Model
{
public:
	Model(const glm::vec3& pos, const std::string& name, bool isDeletable = true,
		bool isVirtual = false);
	virtual ~Model() = default;

	virtual void render() const = 0;
	virtual void updateGUI() = 0;

	glm::vec3 getPos() const;
	virtual void setPos(const glm::vec3& pos);
	float getPitchRad() const;
	virtual void setPitchRad(float pitchRad);
	float getYawRad() const;
	virtual void setYawRad(float yawRad);
	float getRollRad() const;
	virtual void setRollRad(float rollRad);
	glm::vec3 getScale() const;
	virtual void setScale(const glm::vec3& scale);

	std::string getOriginalName() const;
	std::string getName() const;
	void setName(const std::string& name);

	bool isSelected() const;
	bool isDeletable() const;
	bool isVirtual() const;
	virtual void select();
	void deselect();

	glm::mat4 getRotationMatrix() const;

protected:
	glm::mat4 getModelMatrix() const;
	virtual void updateShaders() const = 0;

	void setDeletable(bool deletable);

private:
	const std::string m_originalName{};
	std::string m_name{};

	glm::mat4 m_modelMatrix{};
	glm::vec3 m_pos{};
	float m_pitchRad = 0;
	float m_yawRad = 0;
	float m_rollRad = 0;
	glm::vec3 m_scale = {1, 1, 1};

	bool m_isSelected = false;
	bool m_isVirtual{};
	bool m_isDeletable{};

	void updateModelMatrix();
};
