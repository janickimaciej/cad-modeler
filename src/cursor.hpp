#pragma once

#include "guis/cursor_gui.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

class Cursor
{
public:
	Cursor();
	void render(const ShaderProgram& shaderProgram) const;
	CursorGUI& getGUI();

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& position);

private:
	CursorGUI m_gui;

	glm::vec3 m_position{0, 0, 0};

	unsigned int m_VAO{};

	void updateShaders(const ShaderProgram& shaderProgram) const;
};
