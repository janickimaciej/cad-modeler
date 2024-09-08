#include "meshes/vectorsmesh.hpp"

#include <glad/glad.h>

VectorsMesh::VectorsMesh(const std::array<glm::vec3, m_vertexCount>& vertices)
{
	createVBO(vertices);
	createVAO();
}

VectorsMesh::~VectorsMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void VectorsMesh::update(const std::array<glm::vec3, m_vertexCount>& vertices)
{
	updateVBO(vertices);
}

void VectorsMesh::render() const
{
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_LINES, 0, m_vertexCount);
	glBindVertexArray(0);
}

void VectorsMesh::createVBO(const std::array<glm::vec3, m_vertexCount>& vertices)
{
	glGenBuffers(1, &m_VBO);
	updateVBO(vertices);
}

void VectorsMesh::createVAO()
{
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void VectorsMesh::updateVBO(const std::array<glm::vec3, m_vertexCount>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_DYNAMIC_DRAW);
}
