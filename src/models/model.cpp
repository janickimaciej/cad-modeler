#include "models/model.hpp"

Model::Model(const glm::vec3& pos, const std::string& name, bool isVirtual) :
	m_pos{pos},
	m_originalName{name},
	m_name{name},
	m_isVirtual{isVirtual}
{
	updateMatrix();
}

glm::vec3 Model::getPos() const
{
	return m_pos;
}

void Model::setPos(const glm::vec3& pos)
{
	m_pos = pos;
	updateMatrix();
}

glm::vec2 Model::getScreenPos(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize) const
{
	glm::vec4 clipPos = cameraMatrix * glm::vec4{m_pos, 1};
	clipPos /= clipPos.w;
	return glm::vec2
	{
		(clipPos.x + 1) / 2 * windowSize.x,
		(-clipPos.y + 1) / 2 * windowSize.y
	};
}

void Model::setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
	const glm::ivec2& windowSize)
{
	glm::vec4 prevClipPos = cameraMatrix * glm::vec4{m_pos, 1};
	prevClipPos /= prevClipPos.w;
	glm::vec4 clipPos
	{
		screenPos.x / windowSize.x * 2 - 1,
		-screenPos.y / windowSize.y * 2 + 1,
		prevClipPos.z,
		1
	};
	glm::vec4 worldPos = glm::inverse(cameraMatrix) * clipPos;
	worldPos /= worldPos.w;
	m_pos = glm::vec3{worldPos};
	updateMatrix();
}

float Model::getYawRad() const
{
	return m_yawRad;
}

void Model::setYawRad(float yawRad)
{
	m_yawRad = yawRad;
	updateMatrix();
}

float Model::getPitchRad() const
{
	return m_pitchRad;
}

void Model::setPitchRad(float pitchRad)
{
	m_pitchRad = pitchRad;
	updateMatrix();
}

float Model::getRollRad() const
{
	return m_rollRad;
}

void Model::setRollRad(float rollRad)
{
	m_rollRad = rollRad;
	updateMatrix();
}

glm::vec3 Model::getScale() const
{
	return m_scale;
}

void Model::setScale(const glm::vec3& scale)
{
	m_scale = scale;
	updateMatrix();
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

bool Model::isVirtual() const
{
	return m_isVirtual;
}

bool Model::isActive() const
{
	return m_isActive;
}

void Model::setIsActive(bool isActive)
{
	m_isActive = isActive;
}

float Model::screenDistanceSquared(const glm::vec2& screenRefPos, const glm::mat4& cameraMatrix,
	const glm::ivec2& windowSize) const
{
	glm::vec2 screenPos = getScreenPos(cameraMatrix, windowSize);
	return (screenPos.x - screenRefPos.x) * (screenPos.x - screenRefPos.x) +
		(screenPos.y - screenRefPos.y) * (screenPos.y - screenRefPos.y);
}

glm::mat4 Model::getRotationMatrix() const
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

	glm::mat4 posMatrix
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			m_pos.x, m_pos.y, m_pos.z, 1
		};

	m_modelMatrix =
		posMatrix * getRotationMatrix() * scaleMatrix;
}
