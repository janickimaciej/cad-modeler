#include "meshes/torusMesh.hpp"

#include <glad/glad.h>

TorusMesh::TorusMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	glGenVertexArrays(1, &m_VAO);
	createVBO();
	createEBO();
	update(vertices, indices);
}

TorusMesh::~TorusMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void TorusMesh::update(const std::vector<Vertex>& vertices,
	const std::vector<unsigned int>& indices)
{
	m_indexCount = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
		vertices.data(), GL_DYNAMIC_DRAW);
}

void TorusMesh::render() const
{
	bindVAO();
	glDrawElements(GL_LINES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
	unbindVAO();
}

void TorusMesh::bindVAO() const
{
	glBindVertexArray(m_VAO);
}

void TorusMesh::unbindVAO() const
{
	glBindVertexArray(0);
}

void TorusMesh::createVBO()
{
	glGenBuffers(1, &m_VBO);

	bindVAO();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, surfacePos)));
	glEnableVertexAttribArray(1);
	unbindVAO();
}

void TorusMesh::createEBO()
{
	glGenBuffers(1, &m_EBO);

	bindVAO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	unbindVAO();
}
