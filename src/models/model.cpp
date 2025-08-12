#include "models/model.hpp"

#include <cmath>

Model::Model(const glm::vec3& pos, const std::string& name, bool isDeletable, bool isVirtual) :
	m_pos{pos},
	m_originalName{name},
	m_name{name},
	m_isDeletable{isDeletable},
	m_isVirtual{isVirtual}
{
	updateModelMatrix();
}

glm::vec3 Model::getPos() const
{
	return m_pos;
}

void Model::setPos(const glm::vec3& pos)
{
	m_pos = pos;
	updateModelMatrix();
}

float Model::getYawRad() const
{
	return m_yawRad;
}

void Model::setYawRad(float yawRad)
{
	m_yawRad = yawRad;
	updateModelMatrix();
}

float Model::getPitchRad() const
{
	return m_pitchRad;
}

void Model::setPitchRad(float pitchRad)
{
	m_pitchRad = pitchRad;
	updateModelMatrix();
}

float Model::getRollRad() const
{
	return m_rollRad;
}

void Model::setRollRad(float rollRad)
{
	m_rollRad = rollRad;
	updateModelMatrix();
}

glm::vec3 Model::getScale() const
{
	return m_scale;
}

void Model::setScale(const glm::vec3& scale)
{
	m_scale = scale;
	updateModelMatrix();
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

bool Model::isSelected() const
{
	return m_isSelected;
}

bool Model::isDeletable() const
{
	return m_isDeletable;
}

bool Model::isVirtual() const
{
	return m_isVirtual;
}

void Model::select()
{
	m_isSelected = true;
}

void Model::deselect()
{
	m_isSelected = false;
}

glm::mat4 Model::getRotationMatrix() const
{
	glm::mat4 rotationPitchMatrix
	{
		1, 0, 0, 0,
		0, std::cos(m_pitchRad), std::sin(m_pitchRad), 0,
		0, -std::sin(m_pitchRad), std::cos(m_pitchRad), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationYawMatrix
	{
		std::cos(m_yawRad), 0, -std::sin(m_yawRad), 0,
		0, 1, 0, 0,
		std::sin(m_yawRad), 0, std::cos(m_yawRad), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationRollMatrix
	{
		std::cos(m_rollRad), std::sin(m_rollRad), 0, 0,
		-std::sin(m_rollRad), std::cos(m_rollRad), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	return rotationRollMatrix * rotationYawMatrix * rotationPitchMatrix;
}

glm::mat4 Model::getModelMatrix() const
{
	return m_modelMatrix;
}

void Model::setDeletable(bool deletable)
{
	m_isDeletable = deletable;
}

void Model::updateModelMatrix()
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

	m_modelMatrix = posMatrix * getRotationMatrix() * scaleMatrix;
}
