#include "meshes/bezierCurveMesh.hpp"

#include <glad/glad.h>

BezierCurveMesh::BezierCurveMesh(const std::vector<glm::vec3>& vertices)
{
	createVBO(vertices);
	createVAO();
}

BezierCurveMesh::~BezierCurveMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void BezierCurveMesh::update(const std::vector<glm::vec3>& vertices)
{
	m_vertexCount = vertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveMesh::render() const
{
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(m_vertexCount));
	glBindVertexArray(0);
}

void BezierCurveMesh::createVBO(const std::vector<glm::vec3>& vertices)
{
	glGenBuffers(1, &m_VBO);
	update(vertices);
}

void BezierCurveMesh::createVAO()
{
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
