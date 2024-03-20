#include "center_point.hpp"

CenterPoint::CenterPoint() :
	m_gui{*this}
{
	glGenVertexArrays(1, &m_VAO);
}

void CenterPoint::render(const ShaderProgram& shaderProgram) const
{
	if (m_models.size() == 0)
	{
		return;
	}

	updateShaders(shaderProgram);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

CenterPointGUI& CenterPoint::getGUI()
{
	return m_gui;
}

void CenterPoint::setModels(const std::vector<Model*>& activeModels)
{
	m_models = activeModels;

	if (activeModels.size() != 0)
	{
		glm::vec3 activeModelsPositionsSum{0, 0, 0};
		for (const Model* model : m_models)
		{
			if (model->isActive())
			{
				activeModelsPositionsSum += model->getPosition();
			}
		}
		m_position = activeModelsPositionsSum / static_cast<float>(activeModels.size());
	}
	else
	{
		m_position = glm::vec3{0, 0, 0};
	}
}

glm::vec3 CenterPoint::getPosition() const
{
	return m_position;
}

void CenterPoint::setPosition(const glm::vec3& position)
{
	if (m_models.size() > 0)
	{
		glm::vec3 offset = position - getPosition();
		for (Model* model : m_models)
		{
			model->setPosition(model->getPosition() + offset);
		}
		m_position = position;
	}
}

void CenterPoint::updateShaders(const ShaderProgram& shaderProgram) const
{
	shaderProgram.setUniform3f("posWorld", m_position);
}
