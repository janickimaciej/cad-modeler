#include "centerPoint.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>

CenterPoint::CenterPoint() :
	m_gui{*this}
{
	glGenVertexArrays(1, &m_VAO);
}

void CenterPoint::render(const ShaderProgram& shaderProgram)
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

void CenterPoint::updateGUI()
{
	m_gui.update();
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

void CenterPoint::deleteAllModels()
{
	m_models.clear();
}

glm::vec3 CenterPoint::getPos() const
{
	return m_pos;
}

void CenterPoint::setPos(const glm::vec3& pos)
{
	updatePos();
	if (m_models.size() > 0)
	{
		glm::vec3 posChange = pos - m_pos;
		for (Model* model : m_models)
		{
			model->setPos(model->getPos() + posChange);
		}
	}
}

void CenterPoint::rotateX(float angleRad)
{
	if (m_models.size() > 0)
	{
		glm::mat3 rotationMatrix
		{
			1, 0, 0,
			0, std::cos(angleRad), std::sin(angleRad),
			0, -std::sin(angleRad), std::cos(angleRad)
		};

		rotate(rotationMatrix);
	}
}

void CenterPoint::rotateY(float angleRad)
{
	if (m_models.size() > 0)
	{
		glm::mat3 rotationMatrix
		{
			std::cos(angleRad), 0, -std::sin(angleRad),
			0, 1, 0,
			std::sin(angleRad), 0, std::cos(angleRad)
		};

		rotate(rotationMatrix);
	}
}

void CenterPoint::rotateZ(float angleRad)
{
	if (m_models.size() > 0)
	{
		glm::mat3 rotationMatrix
		{
			std::cos(angleRad), std::sin(angleRad), 0,
			-std::sin(angleRad), std::cos(angleRad), 0,
			0, 0, 1
		};

		rotate(rotationMatrix);
	}
}

void CenterPoint::scaleX(float scale)
{
	updatePos();
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePos = model->getPos() - m_pos;
			model->setPos(m_pos + glm::vec3{scale * relativePos.x, relativePos.y, relativePos.z});
		}
	}
}

void CenterPoint::scaleY(float scale)
{
	updatePos();
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePos = model->getPos() - m_pos;
			model->setPos(m_pos + glm::vec3{relativePos.x, scale * relativePos.y, relativePos.z});
		}
	}
}

void CenterPoint::scaleZ(float scale)
{
	updatePos();
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePos = model->getPos() - m_pos;
			model->setPos(m_pos + glm::vec3{relativePos.x, relativePos.y, scale * relativePos.z});
		}
	}
}

void CenterPoint::rotate(const glm::mat3& rotationMatrix)
{
	for (Model* model : m_models)
	{
		glm::vec3 euler = matrixToEuler(rotationMatrix * glm::mat3{model->getRotationMatrix()});
		model->setPitchRad(euler.x);
		model->setYawRad(euler.y);
		model->setRollRad(euler.z);
		glm::vec3 relativePos = model->getPos() - m_pos;
		model->setPos(m_pos + rotationMatrix * relativePos);
	}
}

void CenterPoint::updatePos()
{
	if (m_models.size() != 0)
	{
		glm::vec3 modelsPosSum{0, 0, 0};
		for (const Model* model : m_models)
		{
			modelsPosSum += model->getPos();
		}
		m_pos = modelsPosSum / static_cast<float>(m_models.size());
	}
	else
	{
		m_pos = glm::vec3{0, 0, 0};
	}
}

void CenterPoint::updateShaders(const ShaderProgram& shaderProgram)
{
	updatePos();
	shaderProgram.setUniform("posWorld", m_pos);
}

glm::vec3 CenterPoint::matrixToEuler(const glm::mat3& rotationMatrix)
{
	glm::vec3 euler{};
	if (rotationMatrix[0][2] != -1 && rotationMatrix[0][2] != 1)
	{
		euler.y = -std::asin(rotationMatrix[0][2]);
		euler.x = std::atan2(rotationMatrix[1][2] / std::cos(euler.y),
			rotationMatrix[2][2] / std::cos(euler.y));
		euler.z = std::atan2(rotationMatrix[0][1] / std::cos(euler.y),
			rotationMatrix[0][0] / std::cos(euler.y));
	}
	else
	{
		static constexpr float pi = glm::pi<float>();
		euler.z = 0;
		if (rotationMatrix[0][2] == -1)
		{
			euler.y = pi / 2;
			euler.x = euler.z + std::atan2(rotationMatrix[1][0], rotationMatrix[2][0]);
		}
		else
		{
			euler.y = -pi / 2;
			euler.x = -euler.z + std::atan2(-rotationMatrix[1][0], -rotationMatrix[2][0]);
		}
	}
	return euler;
}
