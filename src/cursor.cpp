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

glm::vec3 Cursor::getPosition() const
{
	return m_position;
}

void Cursor::setPosition(const glm::vec3& position)
{
	m_position = position;
}

glm::vec2 Cursor::getScreenPosition(const glm::mat4& cameraMatrix,
	const glm::ivec2& windowSize) const
{
	glm::vec4 clipPosition = cameraMatrix * glm::vec4{m_position, 1};
	clipPosition /= clipPosition.w;
	return glm::vec2
	{
		(clipPosition.x + 1) / 2 * windowSize.x,
		(clipPosition.y + 1) / 2 * windowSize.y
	};
}

void Cursor::setScreenPosition(const glm::vec2& screenPosition, const glm::mat4& cameraMatrix,
	const glm::ivec2& windowSize)
{
	glm::vec4 prevClipPosition = cameraMatrix * glm::vec4{m_position, 1};
	prevClipPosition /= prevClipPosition.w;
	glm::vec4 clipPosition
	{
		screenPosition.x / windowSize.x * 2 - 1,
		screenPosition.y / windowSize.y * 2 - 1,
		prevClipPosition.z,
		1
	};
	glm::vec4 worldPosition = glm::inverse(cameraMatrix) * clipPosition;
	worldPosition /= worldPosition.w;
	m_position = glm::vec3{worldPosition};
}

void Cursor::updateShaders(const ShaderProgram& shaderProgram) const
{
	shaderProgram.setUniform("posWorld", m_position);
}
