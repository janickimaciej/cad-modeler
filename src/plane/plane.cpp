#include "plane/plane.hpp"

Plane::Plane(const ShaderProgram& shaderProgram, float scale) :
	m_shaderProgram{shaderProgram},
	m_scale{scale}
{ }

void Plane::render(CameraType cameraType) const
{
	updateShaders(cameraType);
	m_quad.render();
}

void Plane::updateShaders(CameraType cameraType) const
{
	m_shaderProgram.setUniform("scale", m_scale);
	m_shaderProgram.setUniform("cameraType", static_cast<int>(cameraType));
}
