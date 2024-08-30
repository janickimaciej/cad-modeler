#pragma once

#include "cameras/cameraType.hpp"
#include "quad.hpp"
#include "shaderProgram.hpp"

class Grid
{
public:
	Grid(const ShaderProgram& shaderProgram, float scale);
	void render(CameraType cameraType) const;

private:
	const ShaderProgram& m_shaderProgram;

	float m_scale{};
	Quad m_quad{};

	void updateShaders(CameraType cameraType) const;
};
