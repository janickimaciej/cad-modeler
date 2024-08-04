#include "cursor.hpp"

#include <glad/glad.h>

#include <array>

Cursor::Cursor() :
	m_gui{*this}
{
	glGenVertexArrays(1, &m_VAO);
}

Cursor::~Cursor()
{
	glDeleteVertexArrays(1, &m_VAO);
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

glm::vec3 Cursor::getPos() const
{
	return m_pos;
}

void Cursor::setPos(const glm::vec3& pos)
{
	m_pos = pos;
}

glm::vec2 Cursor::getScreenPos(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize) const
{
	glm::vec4 clipPos = cameraMatrix * glm::vec4{m_pos, 1};
	clipPos /= clipPos.w;
	return glm::vec2
	{
		(clipPos.x + 1) / 2 * windowSize.x,
		(clipPos.y + 1) / 2 * windowSize.y
	};
}

void Cursor::setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
	const glm::ivec2& windowSize)
{
	glm::vec4 prevClipPos = cameraMatrix * glm::vec4{m_pos, 1};
	prevClipPos /= prevClipPos.w;
	glm::vec4 clipPos
	{
		screenPos.x / windowSize.x * 2 - 1,
		screenPos.y / windowSize.y * 2 - 1,
		prevClipPos.z,
		1
	};
	glm::vec4 worldPos = glm::inverse(cameraMatrix) * clipPos;
	worldPos /= worldPos.w;
	m_pos = glm::vec3{worldPos};
}

void Cursor::updateShaders(const ShaderProgram& shaderProgram) const
{
	shaderProgram.setUniform("posWorld", m_pos);
}
