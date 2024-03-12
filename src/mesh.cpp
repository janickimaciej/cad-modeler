#include "mesh.hpp"

#include <glad/glad.h>

#include <vector>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indicesWireframe,
	const std::vector<unsigned int>& indicesSolid)
{
	m_indexCountWireframe = indicesWireframe.size();
	createBuffersWireframe(vertices, indicesWireframe);

	m_indexCountSolid = indicesSolid.size();
	createBuffersSolid(vertices, indicesSolid);
}

void Mesh::render(RenderMode renderMode) const
{
	switch (renderMode)
	{
	case RenderMode::wireframe:
		glBindVertexArray(m_VAOWireframe);
		glDrawElements(GL_LINES, m_indexCountWireframe, GL_UNSIGNED_INT, nullptr);
		break;

	case RenderMode::solid:
		glBindVertexArray(m_VAOSolid);
		glDrawElements(GL_TRIANGLES, m_indexCountSolid, GL_UNSIGNED_INT, nullptr);
		break;
	}
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAOSolid);
	glDeleteVertexArrays(1, &m_VAOWireframe);
	glDeleteBuffers(1, &m_EBOSolid);
	glDeleteBuffers(1, &m_EBOWireframe);
	glDeleteBuffers(1, &m_VBO);
}

void Mesh::createBuffersWireframe(const std::vector<Vertex>& vertices,
	const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBOWireframe);
	glGenVertexArrays(1, &m_VAOWireframe);

	glBindVertexArray(m_VAOWireframe);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
		vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOWireframe);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Mesh::createBuffersSolid(const std::vector<Vertex>& vertices,
	const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBOSolid);
	glGenVertexArrays(1, &m_VAOSolid);

	glBindVertexArray(m_VAOSolid);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
		vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOSolid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, normalVector));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
