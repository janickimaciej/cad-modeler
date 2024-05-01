#include "center_point.hpp"

#include <glm/gtc/constants.hpp>

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

std::vector<Model*> CenterPoint::getModels() const
{
	return m_models;
}

int CenterPoint::getModelCount() const
{
	return static_cast<int>(m_models.size());
}

void CenterPoint::addModel(Model* model)
{
	m_models.push_back(model);

	updatePosition();
}

void CenterPoint::deleteModel(const Model* model)
{
	std::erase_if(m_models,
		[deletedModel = model] (Model* model)
		{
			return model == deletedModel;
		}
	);
}

void CenterPoint::clearModels()
{
	m_models.clear();

	updatePosition();
}

glm::vec3 CenterPoint::getPosition()
{
	updatePosition();

	return m_position;
}

void CenterPoint::setPosition(const glm::vec3& position)
{
	if (m_models.size() > 0)
	{
		glm::vec3 positionChange = position - m_position;
		for (Model* model : m_models)
		{
			model->setPosition(m_position + positionChange);
		}
		m_position = position;
	}
}

void CenterPoint::rotateX(float angleRad)
{
	if (m_models.size() > 0)
	{
		glm::mat3 rotationMatrix
		{
			1, 0, 0,
			0, cos(angleRad), sin(angleRad),
			0, -sin(angleRad), cos(angleRad)
		};

		for (Model* model : m_models)
		{
			glm::vec3 euler = matrixToEuler(rotationMatrix * model->getRotationMatrix());
			model->setPitchRad(euler.x);
			model->setYawRad(euler.y);
			model->setRollRad(euler.z);
			glm::vec3 relativePosition = model->getPosition() - m_position;
			model->setPosition(m_position + rotationMatrix * relativePosition);
		}
	}
}

void CenterPoint::rotateY(float angleRad)
{
	if (m_models.size() > 0)
	{
		glm::mat3 rotationMatrix
		{
			cos(angleRad), 0, -sin(angleRad),
			0, 1, 0,
			sin(angleRad), 0, cos(angleRad)
		};

		for (Model* model : m_models)
		{
			glm::vec3 euler = matrixToEuler(rotationMatrix * model->getRotationMatrix());
			model->setPitchRad(euler.x);
			model->setYawRad(euler.y);
			model->setRollRad(euler.z);
			glm::vec3 relativePosition = model->getPosition() - m_position;
			model->setPosition(m_position + rotationMatrix * relativePosition);
		}
	}
}

void CenterPoint::rotateZ(float angleRad)
{
	if (m_models.size() > 0)
	{
		glm::mat3 rotationMatrix
		{
			cos(angleRad), sin(angleRad), 0,
			-sin(angleRad), cos(angleRad), 0,
			0, 0, 1
		};

		for (Model* model : m_models)
		{
			glm::vec3 euler = matrixToEuler(rotationMatrix * model->getRotationMatrix());
			model->setPitchRad(euler.x);
			model->setYawRad(euler.y);
			model->setRollRad(euler.z);
			glm::vec3 relativePosition = model->getPosition() - m_position;
			model->setPosition(m_position + rotationMatrix * relativePosition);
		}
	}
}

void CenterPoint::scaleX(float scale)
{
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePosition = model->getPosition() - m_position;
			model->setPosition(m_position +
				glm::vec3{scale * relativePosition.x, relativePosition.y, relativePosition.z});
		}
	}
}

void CenterPoint::scaleY(float scale)
{
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePosition = model->getPosition() - m_position;
			model->setPosition(m_position +
				glm::vec3{relativePosition.x, scale * relativePosition.y, relativePosition.z});
		}
	}
}

void CenterPoint::scaleZ(float scale)
{
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePosition = model->getPosition() - m_position;
			model->setPosition(m_position +
				glm::vec3{relativePosition.x, relativePosition.y, scale * relativePosition.z});
		}
	}
}

void CenterPoint::updatePosition()
{
	if (m_models.size() != 0)
	{
		glm::vec3 modelsPositionsSum{0, 0, 0};
		for (const Model* model : m_models)
		{
			modelsPositionsSum += model->getPosition();
		}
		m_position = modelsPositionsSum / static_cast<float>(m_models.size());
	}
	else
	{
		m_position = glm::vec3{0, 0, 0};
	}
}

void CenterPoint::updateShaders(const ShaderProgram& shaderProgram) const
{
	shaderProgram.setUniform("posWorld", m_position);
}

glm::vec3 CenterPoint::matrixToEuler(const glm::mat3& rotationMatrix)
{
	glm::vec3 euler{};
	if (rotationMatrix[0][2] != -1 && rotationMatrix[0][2] != 1)
	{
		euler.y = -asin(rotationMatrix[0][2]);
		euler.x = atan2(rotationMatrix[1][2] / cos(euler.y), rotationMatrix[2][2] / cos(euler.y));
		euler.z = atan2(rotationMatrix[0][1] / cos(euler.y), rotationMatrix[0][0] / cos(euler.y));
	}
	else
	{
		euler.z = 0;
		if (rotationMatrix[0][2] == -1)
		{
			euler.y = glm::pi<float>() / 2;
			euler.x = euler.z + atan2(rotationMatrix[1][0], rotationMatrix[2][0]);
		}
		else
		{
			euler.y = -glm::pi<float>() / 2;
			euler.x = -euler.z + atan2(-rotationMatrix[1][0], -rotationMatrix[2][0]);
		}
	}
	return euler;
}
