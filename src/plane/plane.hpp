#pragma once

#include "cameras/cameraType.hpp"
#include "quad.hpp"
#include "shaderProgram.hpp"
#include "shaderPrograms.hpp"

class Plane
{
public:
	Plane(float scale);
	void render(CameraType cameraType) const;

private:
	const ShaderProgram& m_shaderProgram = *ShaderPrograms::plane;

	float m_scale{};
	Quad m_quad{};

	void updateShaders(CameraType cameraType) const;
};
