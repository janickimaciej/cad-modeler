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

void Model::updateMatrix()
{
	float xx = m_orientation.x * m_orientation.x;
	float xy = m_orientation.x * m_orientation.y;
	float xz = m_orientation.x * m_orientation.z;
	float xw = m_orientation.x * m_orientation.w;
	float yy = m_orientation.y * m_orientation.y;
	float yz = m_orientation.y * m_orientation.z;
	float yw = m_orientation.y * m_orientation.w;
	float zz = m_orientation.z * m_orientation.z;
	float zw = m_orientation.z * m_orientation.w;

	glm::mat4 positionOrientationMatrix
		{
			1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw), 0,
			2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw), 0,
			2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy), 0,
			m_position.x, m_position.y, m_position.z, 1
		};

	glm::mat4 scaleMatrix
		{
			m_scale.x, 0, 0, 0,
			0, m_scale.y, 0, 0,
			0, 0, m_scale.z, 0,
			0, 0, 0, 1
		};

	m_modelMatrix = positionOrientationMatrix * scaleMatrix;
}
