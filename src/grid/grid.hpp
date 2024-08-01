#pragma once

#include "cameraType.hpp"
#include "grid/canvas.hpp"
#include "shaderProgram.hpp"

class Grid
{
public:
	Grid(float scale);
	void render(const ShaderProgram& shaderProgram, CameraType cameraType) const;

private:
	float m_scale{};
	Canvas m_canvas{};

	void updateShaders(const ShaderProgram& shaderProgram, CameraType cameraType) const;
};
