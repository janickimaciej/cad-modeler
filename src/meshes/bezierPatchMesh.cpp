#include "meshes/bezierPatchMesh.hpp"

#include <glad/glad.h>

BezierPatchMesh::BezierPatchMesh(const std::array<glm::vec3, 16>& vertices)
{
	createVBO(vertices);
	createVAOs();
}

BezierPatchMesh::~BezierPatchMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void BezierPatchMesh::update(const std::array<glm::vec3, 16>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_DYNAMIC_DRAW);
}

void BezierPatchMesh::render() const
{
	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_PATCHES, 0, 16);
	glBindVertexArray(0);
}

void BezierPatchMesh::createVBO(const std::array<glm::vec3, 16>& vertices)
{
	glGenBuffers(1, &m_VBO);
	update(vertices);
}

void BezierPatchMesh::createVAOs()
{
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
