#include "centerPoint.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>

CenterPoint::CenterPoint(const ShaderProgram& shaderProgram, const std::vector<Model*>& models) :
	m_shaderProgram{shaderProgram},
	m_models{models}
{ }

void CenterPoint::render() const
{
	if (m_models.size() == 0)
	{
		return;
	}

	updateShaders();
	m_mesh.render();
}

void CenterPoint::updateGUI()
{
	m_gui.update();
}

glm::vec3 CenterPoint::getPos() const
{
	if (m_models.size() == 0)
	{
		return glm::vec3{0, 0, 0};
	}

	glm::vec3 modelsPosSum{0, 0, 0};
	for (const Model* model : m_models)
	{
		modelsPosSum += model->getPos();
	}
	return modelsPosSum / static_cast<float>(m_models.size());
}

void CenterPoint::setPos(const glm::vec3& newPos)
{
	if (m_models.size() > 0)
	{
		glm::vec3 posChange = newPos - getPos();
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
	glm::vec3 pos = getPos();
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePos = model->getPos() - pos;
			model->setPos(pos + glm::vec3{scale * relativePos.x, relativePos.y, relativePos.z});
		}
	}
}

void CenterPoint::scaleY(float scale)
{
	glm::vec3 pos = getPos();
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePos = model->getPos() - pos;
			model->setPos(pos + glm::vec3{relativePos.x, scale * relativePos.y, relativePos.z});
		}
	}
}

void CenterPoint::scaleZ(float scale)
{
	glm::vec3 pos = getPos();
	if (m_models.size() > 0)
	{
		for (Model* model : m_models)
		{
			glm::vec3 relativePos = model->getPos() - pos;
			model->setPos(pos + glm::vec3{relativePos.x, relativePos.y, scale * relativePos.z});
		}
	}
}

void CenterPoint::rotate(const glm::mat3& rotationMatrix)
{
	glm::vec3 pos = getPos();
	for (Model* model : m_models)
	{
		glm::vec3 euler = matrixToEuler(rotationMatrix * glm::mat3{model->getRotationMatrix()});
		model->setPitchRad(euler.x);
		model->setYawRad(euler.y);
		model->setRollRad(euler.z);
		glm::vec3 relativePos = model->getPos() - pos;
		model->setPos(pos + rotationMatrix * relativePos);
	}
}

void CenterPoint::updateShaders() const
{
	m_shaderProgram.setUniform("posWorld", getPos());
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
