#include "meshes/bezierSurfaceMesh.hpp"

#include <glad/glad.h>

BezierSurfaceMesh::BezierSurfaceMesh(const std::vector<glm::vec3>& vertices,
	const std::vector<std::vector<unsigned int>>& indices)
{
	m_patchCount = indices.size();
	createVBO(vertices);
	createEBOs(indices);
	createVAOs();
}

BezierSurfaceMesh::~BezierSurfaceMesh()
{
	glDeleteVertexArrays(static_cast<GLsizei>(m_VAOs.size()), m_VAOs.data());
	glDeleteBuffers(static_cast<GLsizei>(m_EBOs.size()), m_EBOs.data());
	glDeleteBuffers(1, &m_VBO);
}

void BezierSurfaceMesh::update(const std::vector<glm::vec3>& vertices,
	const std::vector<std::vector<unsigned int>>& indices)
{
	updateVBO(vertices);
	updateEBOs(indices);
}

void BezierSurfaceMesh::render(std::size_t patch) const
{
	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glBindVertexArray(m_VAOs[patch]);
	glDrawElements(GL_PATCHES, 16, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void BezierSurfaceMesh::createVBO(const std::vector<glm::vec3>& vertices)
{
	glGenBuffers(1, &m_VBO);
	updateVBO(vertices);
}

void BezierSurfaceMesh::createEBOs(const std::vector<std::vector<unsigned int>>& indices)
{
	m_EBOs.resize(m_patchCount);
	glGenBuffers(static_cast<GLsizei>(m_patchCount), m_EBOs.data());
	updateEBOs(indices);
}

void BezierSurfaceMesh::createVAOs()
{
	m_VAOs.resize(m_patchCount);
	glGenVertexArrays(static_cast<GLsizei>(m_patchCount), m_VAOs.data());

	for (std::size_t patch = 0; patch < m_patchCount; ++patch)
	{
		glBindVertexArray(m_VAOs[patch]);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOs[patch]);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}
}

void BezierSurfaceMesh::updateVBO(const std::vector<glm::vec3>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
		vertices.data(), GL_DYNAMIC_DRAW);
}

void BezierSurfaceMesh::updateEBOs(const std::vector<std::vector<unsigned int>>& indices)
{
	for (std::size_t patch = 0; patch < m_patchCount; ++patch)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOs[patch]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			static_cast<GLsizeiptr>(indices[patch].size() * sizeof(unsigned int)),
			indices[patch].data(), GL_DYNAMIC_DRAW);
	}
}
