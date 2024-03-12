#include "models/model.hpp"

Model::Model()
{
	updateMatrix();
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

	m_modelMatrix =
		glm::mat4
		{
			1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw), 0,
			2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw), 0,
			2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy), 0,
			m_position.x, m_position.y, m_position.z, 1
		};
}

void Model::updateShaders(const ShaderProgram& shaderProgram) const
{
	shaderProgram.setUniformMatrix4f("modelMatrix", m_modelMatrix);
}
