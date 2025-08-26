#include "meshes/flatMesh.hpp"

#include <glad/glad.h>

FlatMesh::FlatMesh(const std::vector<glm::vec2>& vertices, const std::vector<unsigned int>& indices)
{
	glGenVertexArrays(1, &m_VAO);
	createVBO(vertices);
	createEBO(indices);
}

FlatMesh::~FlatMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void FlatMesh::render() const
{
	bindVAO();
	glDrawElements(GL_LINES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
	unbindVAO();
}

void FlatMesh::bindVAO() const
{
	glBindVertexArray(m_VAO);
}

void FlatMesh::unbindVAO() const
{
	glBindVertexArray(0);
}

void FlatMesh::createVBO(const std::vector<glm::vec2>& vertices)
{
	glGenBuffers(1, &m_VBO);

	bindVAO();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	unbindVAO();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec2)),
		vertices.data(), GL_STATIC_DRAW);
}

void FlatMesh::createEBO(const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &m_EBO);

	bindVAO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	unbindVAO();

	m_indexCount = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_STATIC_DRAW);
}
