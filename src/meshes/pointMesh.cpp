#include "meshes/pointMesh.hpp"

#include <glad/glad.h>

PointMesh::PointMesh()
{
	glGenVertexArrays(1, &m_VAO);
}

PointMesh::~PointMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
}

void PointMesh::render() const
{
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
