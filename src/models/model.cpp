#include "models/model.hpp"

Model::Model(const glm::vec3& position, const std::string& name) :
	m_position{position},
	m_originalName{name},
	m_name{name}
{
	updateMatrix();
}

glm::vec3 Model::getPosition() const
{
	return m_position;
}

void Model::setPosition(const glm::vec3& position)
{
	m_position = position;
	updateMatrix();
}

float Model::getYawRad() const
{
	return m_yawRad;
}

void Model::setYawRad(float yawRad)
{
	m_yawRad = yawRad;
}

float Model::getPitchRad() const
{
	return m_pitchRad;
}

void Model::setPitchRad(float pitchRad)
{
	m_pitchRad = pitchRad;
}

float Model::getRollRad() const
{
	return m_rollRad;
}

void Model::setRollRad(float rollRad)
{
	m_rollRad = rollRad;
}

glm::vec3 Model::getScale() const
{
	return m_scale;
}

void Model::setScale(const glm::vec3& scale)
{
	m_scale = scale;
}

std::string Model::getOriginalName() const
{
	return m_originalName;
}

std::string Model::getName() const
{
	return m_name;
}

void Model::setName(const std::string& name)
{
	m_name = name;
}

bool Model::isActive() const
{
	return m_isActive;
}

void Model::setIsActive(bool isActive)
{
	m_isActive = isActive;
}

float Model::distanceSquared(float xPos, float yPos, int windowWidth, int windowHeight,
	const glm::mat4& cameraMatrix) const
{
	glm::vec4 clipPosition = cameraMatrix * glm::vec4(m_position, 1);
	clipPosition /= clipPosition.w;
	glm::vec2 screenPosition{(clipPosition.x + 1) / 2 * windowWidth,
		(-clipPosition.y + 1) / 2 * windowHeight};
	return (screenPosition.x - xPos) * (screenPosition.x - xPos) +
		(screenPosition.y - yPos) * (screenPosition.y - yPos);
}

glm::mat3 Model::getRotationMatrix() const
{
	glm::mat4 rotationYawMatrix
	{
		cos(m_yawRad), 0, -sin(m_yawRad), 0,
		0, 1, 0, 0,
		sin(m_yawRad), 0, cos(m_yawRad), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationPitchMatrix
	{
		1, 0, 0, 0,
		0, cos(m_pitchRad), sin(m_pitchRad), 0,
		0, -sin(m_pitchRad), cos(m_pitchRad), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationRollMatrix
	{
		cos(m_rollRad), sin(m_rollRad), 0, 0,
		-sin(m_rollRad), cos(m_rollRad), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	return rotationRollMatrix * rotationYawMatrix * rotationPitchMatrix;
}

void Model::updateMatrix()
{
	glm::mat4 scaleMatrix
		{
			m_scale.x, 0, 0, 0,
			0, m_scale.y, 0, 0,
			0, 0, m_scale.z, 0,
			0, 0, 0, 1
		};

	glm::mat4 rotationYawMatrix
	{
		cos(m_yawRad), 0, -sin(m_yawRad), 0,
		0, 1, 0, 0,
		sin(m_yawRad), 0, cos(m_yawRad), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationPitchMatrix
	{
		1, 0, 0, 0,
		0, cos(m_pitchRad), sin(m_pitchRad), 0,
		0, -sin(m_pitchRad), cos(m_pitchRad), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationRollMatrix
	{
		cos(m_rollRad), sin(m_rollRad), 0, 0,
		-sin(m_rollRad), cos(m_rollRad), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	glm::mat4 positionMatrix
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			m_position.x, m_position.y, m_position.z, 1
		};

	m_modelMatrix =
		positionMatrix * rotationRollMatrix * rotationYawMatrix * rotationPitchMatrix * scaleMatrix;
}
