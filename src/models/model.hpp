#pragma once

#include "model_guis/model_gui.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

class Model
{
public:
	virtual void render(const ShaderProgram& shaderProgram) const = 0;
	virtual ModelGUI& getGUI() = 0;

protected:
	glm::mat4 m_modelMatrix{1};

	void updateShaders(const ShaderProgram& shaderProgram) const;
};
