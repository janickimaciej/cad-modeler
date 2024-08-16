#include "meshes/bezierCurveMesh.hpp"

#include <glad/glad.h>

BezierCurveMesh::BezierCurveMesh(const std::vector<glm::vec3>& vertices,
	const std::vector<unsigned int>& indices)
{
	createVBO(vertices);
	createEBO(indices);
	createVAO();
}

BezierCurveMesh::~BezierCurveMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteBuffers(1, &m_VBO);
}

void BezierCurveMesh::update(const std::vector<glm::vec3>& vertices,
	const std::vector<unsigned int>& indices)
{
	updateVBO(vertices);
	updateEBO(indices);
}

void BezierCurveMesh::render() const
{
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindVertexArray(m_VAO);
	glDrawElements(GL_PATCHES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void BezierCurveMesh::createVBO(const std::vector<glm::vec3>& vertices)
{
	glGenBuffers(1, &m_VBO);
	updateVBO(vertices);
}

void BezierCurveMesh::createEBO(const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &m_EBO);
	updateEBO(indices);
}

void BezierCurveMesh::createVAO()
{
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurveMesh::updateVBO(const std::vector<glm::vec3>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveMesh::updateEBO(const std::vector<unsigned int>& indices)
{
	m_indexCount = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_DYNAMIC_DRAW);
}
