#pragma once

#include "model_guis/model_gui.hpp"
#include "render_mode.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

class Model
{
public:
	Model();
	virtual void render(RenderMode renderMode, const ShaderProgram& shaderProgram) const = 0;
	virtual ModelGUI& getGUI() = 0;

protected:
	glm::mat4 m_modelMatrix{1};

	glm::vec3 m_position{0, 0, 0};
	glm::vec4 m_orientation{0, 0, 0, 1};

	void updateMatrix();
	void updateShaders(const ShaderProgram& shaderProgram) const;
};
