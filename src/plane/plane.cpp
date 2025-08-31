#include "plane/plane.hpp"

Plane::Plane(float scale) :
	m_scale{scale}
{ }

void Plane::render(CameraType cameraType) const
{
	updateShaders(cameraType);
	m_quad.render();
}

void Plane::updateShaders(CameraType cameraType) const
{
	m_shaderProgram.use();
	m_shaderProgram.setUniform("scale", m_scale);
	m_shaderProgram.setUniform("cameraType", static_cast<int>(cameraType));
}
