#include "meshes/mesh.hpp"

Mesh::Mesh(const std::vector<glm::vec3>& vertices, GLenum drawType, GLint patchVertices) :
	m_drawType{drawType},
	m_patchVertices{patchVertices}
{
	glGenVertexArrays(1, &m_VAO);
	createVBO(vertices);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void Mesh::update(const std::vector<glm::vec3>& vertices)
{
	m_vertexCount = vertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_DYNAMIC_DRAW);
}

void Mesh::render() const
{
	if (drawType() == GL_PATCHES)
	{
		glPatchParameteri(GL_PATCH_VERTICES, patchVertices());
	}

	bindVAO();
	glDrawArrays(drawType(), 0, static_cast<GLsizei>(m_vertexCount));
	unbindVAO();
}

void Mesh::bindVAO() const
{
	glBindVertexArray(m_VAO);
}

void Mesh::unbindVAO() const
{
	glBindVertexArray(0);
}

GLenum Mesh::drawType() const
{
	return m_drawType;
}

GLint Mesh::patchVertices() const
{
	return m_patchVertices;
}

void Mesh::createVBO(const std::vector<glm::vec3>& vertices)
{
	glGenBuffers(1, &m_VBO);

	bindVAO();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	unbindVAO();

	update(vertices);
}
