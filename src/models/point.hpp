#pragma once

#include "guis/model_guis/model_gui.hpp"
#include "guis/model_guis/point_gui.hpp"
#include "models/model.hpp"
#include "render_mode.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

class Point : public Model
{
public:
	Point(const ShaderProgram& wireframePointShaderProgram,
		const ShaderProgram& solidPointShaderProgram, glm::vec3 position);
	virtual void render(RenderMode renderMode) const override;
	virtual ModelGUI& getGUI() override;

private:
	static int m_count;
	int m_id{};

	const ShaderProgram& m_wireframePointShaderProgram;
	const ShaderProgram& m_solidPointShaderProgram;
	PointGUI m_gui;

	unsigned int m_VAO{};

	virtual void updateShaders(RenderMode renderMode) const override;
};
