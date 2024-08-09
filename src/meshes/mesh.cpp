#include "meshes/mesh.hpp"

#include <glad/glad.h>

Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices)
{
	createVBO(vertices);
	createEBO(indices);
	createVAO();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteBuffers(1, &m_VBO);
}

void Mesh::update(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices)
{
	updateVBO(vertices);
	updateEBO(indices);
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
	updateVBO(vertices);
}

void Mesh::createEBO(const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &m_EBO);
	updateEBO(indices);
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

void Mesh::updateVBO(const std::vector<glm::vec3>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_STATIC_DRAW);
}

void Mesh::updateEBO(const std::vector<unsigned int>& indices)
{
	m_indexCount = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_STATIC_DRAW);
}
