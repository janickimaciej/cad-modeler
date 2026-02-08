#include "plane/plane.hpp"

#include "shaderPrograms.hpp"

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
	ShaderPrograms::plane->use();
	ShaderPrograms::plane->setUniform("scale", m_scale);
	ShaderPrograms::plane->setUniform("cameraType", static_cast<int>(cameraType));
}
