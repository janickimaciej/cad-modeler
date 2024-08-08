#include "meshes/mesh.hpp"

#include <glad/glad.h>

Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices)
{
	createVBO(vertices);
	m_indexCount = indices.size();
	createEBO(m_EBO, indices);
	createVAO();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteBuffers(1, &m_VBO);
}

void Mesh::render() const
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_LINES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::createVBO(const std::vector<glm::vec3>& vertices)
{
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_STATIC_DRAW);
}

void Mesh::createEBO(unsigned int& EBO, const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_STATIC_DRAW);
}

void Mesh::createVAO()
{
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
