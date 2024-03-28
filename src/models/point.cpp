#include "models/point.hpp"

#include <glad/glad.h>

#include <array>
#include <string>

Point::Point(const ShaderProgram& wireframePointShaderProgram,
	const ShaderProgram& solidPointShaderProgram, glm::vec3 position) :
	Model{position, "Point " + std::to_string(m_count)},
	m_id{m_count++},
	m_wireframePointShaderProgram{wireframePointShaderProgram},
	m_solidPointShaderProgram{solidPointShaderProgram},
	m_gui{*this}
{
	glGenVertexArrays(1, &m_VAO);
}

void Point::render(RenderMode renderMode) const
{
	updateShaders(renderMode);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

ModelGUI& Point::getGUI()
{
	return m_gui;
}

int Point::m_count = 0;

void Point::updateShaders(RenderMode renderMode) const
{
	switch (renderMode)
	{
	case RenderMode::wireframe:
		/*m_wireframePointShaderProgram.use();
		m_wireframePointShaderProgram.setUniform3f("posWorld", m_position);
		m_wireframePointShaderProgram.setUniform1b("isActive", isActive());
		break;*/

	case RenderMode::solid:
		m_solidPointShaderProgram.use();
		m_solidPointShaderProgram.setUniform3f("posWorld", m_position);
		m_solidPointShaderProgram.setUniform1b("isActive", isActive());
		break;
	}
}
