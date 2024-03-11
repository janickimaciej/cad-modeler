#include "models/model.hpp"

void Model::updateShaders(const ShaderProgram& shaderProgram) const
{
	shaderProgram.setUniformMatrix4f("modelMatrix", m_modelMatrix);
}
