#include "meshes/gregorySurfaceMesh.hpp"

#include <glad/glad.h>

GregorySurfaceMesh::GregorySurfaceMesh(const std::array<glm::vec3, m_vertexCount>& vertices)
{
	createVBO(vertices);
	createVAO();
}

GregorySurfaceMesh::~GregorySurfaceMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void GregorySurfaceMesh::update(const std::array<glm::vec3, m_vertexCount>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_DYNAMIC_DRAW);
}

void GregorySurfaceMesh::render() const
{
	glPatchParameteri(GL_PATCH_VERTICES, m_patchVertexCount);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_PATCHES, 0, m_vertexCount);
	glBindVertexArray(0);
}

void GregorySurfaceMesh::createVBO(const std::array<glm::vec3, m_vertexCount>& vertices)
{
	glGenBuffers(1, &m_VBO);
	update(vertices);
}

void GregorySurfaceMesh::createVAO()
{
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
