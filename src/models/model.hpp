#pragma once

#include "guis/model_guis/model_gui.hpp"
#include "render_mode.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <string>

class Model
{
public:
	Model(const glm::vec3& position, const std::string& name);
	virtual void render(RenderMode renderMode) const = 0;
	virtual ModelGUI& getGUI() = 0;

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& position);
	glm::vec3 getScale() const;
	void setScale(const glm::vec3& scale);

	std::string getOriginalName() const;
	std::string getName() const;
	void setName(const std::string& name);

	bool isActive() const;
	void setIsActive(bool isActive);

protected:
	glm::mat4 m_modelMatrix{1};

	glm::vec3 m_position{};
	glm::vec4 m_orientation{0, 0, 0, 1};
	glm::vec3 m_scale = {1, 1, 1};

	void updateMatrix();
	virtual void updateShaders(RenderMode renderMode) const = 0;

private:
	const std::string m_originalName{};
	std::string m_name{};

	bool m_isActive = false;
};
