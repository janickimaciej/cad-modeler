#include "grid/grid.hpp"

Grid::Grid(const ShaderProgram& shaderProgram, float scale) :
	m_shaderProgram{shaderProgram},
	m_scale{scale}
{ }

void Grid::render(CameraType cameraType) const
{
	updateShaders(cameraType);
	m_canvas.render();
}

void Grid::updateShaders(CameraType cameraType) const
{
	m_shaderProgram.setUniform("scale", m_scale);
	m_shaderProgram.setUniform("cameraType", static_cast<int>(cameraType));
}
