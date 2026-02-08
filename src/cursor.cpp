#include "cursor.hpp"

#include "shaderPrograms.hpp"

Cursor::Cursor()
{ }

void Cursor::render() const
{
	updateShaders();
	m_mesh.render();
}

void Cursor::updateGUI(const Camera& camera)
{
	m_gui.update(camera);
}

glm::vec3 Cursor::getPos() const
{
	return m_pos;
}

void Cursor::setPos(const glm::vec3& pos)
{
	m_pos = pos;
}

void Cursor::updateShaders() const
{
	ShaderPrograms::cursor->use();
	ShaderPrograms::cursor->setUniform("posWorld", m_pos);
}
