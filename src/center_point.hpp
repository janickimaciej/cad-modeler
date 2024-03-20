#pragma once

#include "guis/center_point_gui.hpp"
#include "models/model.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <vector>

class CenterPoint
{
public:
	CenterPoint();
	void render(const ShaderProgram& shaderProgram) const;
	CenterPointGUI& getGUI();
	void setModels(const std::vector<Model*>& activeModels);

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& position);

private:
	CenterPointGUI m_gui;

	std::vector<Model*> m_models{};

	glm::vec3 m_position{0, 0, 0};

	unsigned int m_VAO{};

	void updateShaders(const ShaderProgram& shaderProgram) const;
};
