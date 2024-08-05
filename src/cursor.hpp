#pragma once

#include "guis/cursorGUI.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

class Cursor
{
public:
	Cursor();
	~Cursor();

	void render(const ShaderProgram& shaderProgram) const;
	void updateGUI(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize);

	glm::vec3 getPos() const;
	void setPos(const glm::vec3& pos);
	glm::vec2 getScreenPos(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize) const;
	void setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize);

private:
	CursorGUI m_gui;

	glm::vec3 m_pos{0, 0, 0};

	unsigned int m_VAO{};

	void updateShaders(const ShaderProgram& shaderProgram) const;
};
