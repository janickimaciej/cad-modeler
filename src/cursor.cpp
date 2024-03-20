#include "cursor.hpp"

#include <glad/glad.h>

#include <array>

Cursor::Cursor() :
	m_gui{*this}
{
	glGenVertexArrays(1, &m_VAO);
}

void Cursor::render(const ShaderProgram& shaderProgram) const
{
	updateShaders(shaderProgram);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

CursorGUI& Cursor::getGUI()
{
	return m_gui;
}

glm::vec3 Cursor::getPosition() const
{
	return m_position;
}

void Cursor::setPosition(const glm::vec3& position)
{
	m_position = position;
}

void Cursor::updateShaders(const ShaderProgram& shaderProgram) const
{
	shaderProgram.setUniform3f("posWorld", m_position);
}
