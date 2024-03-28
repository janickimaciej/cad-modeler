#pragma once

#include "guis/cursor_gui.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

class Scene;

class Cursor
{
public:
	Cursor(const Scene& scene);
	void render(const ShaderProgram& shaderProgram) const;
	CursorGUI& getGUI();

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& position);
	glm::vec2 getScreenPosition() const;
	void setScreenPosition(const glm::vec2& screenPosition);

private:
	const Scene& m_scene;
	CursorGUI m_gui;

	glm::vec3 m_position{0, 0, 0};

	unsigned int m_VAO{};

	void updateShaders(const ShaderProgram& shaderProgram) const;
};
