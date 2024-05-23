#include "mesh.hpp"

#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indicesWireframe,
	const std::vector<unsigned int>& indicesSolid)
{
	createVBO(vertices);

	m_indexCountWireframe = indicesWireframe.size();
	createEBO(m_EBOWireframe, indicesWireframe);
	createVAOWireframe();

	m_indexCountSolid = indicesSolid.size();
	createEBO(m_EBOSolid, indicesSolid);
	createVAOSolid();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAOSolid);
	glDeleteVertexArrays(1, &m_VAOWireframe);
	glDeleteBuffers(1, &m_EBOSolid);
	glDeleteBuffers(1, &m_EBOWireframe);
	glDeleteBuffers(1, &m_VBO);
}

void Mesh::render(RenderMode renderMode) const
{
	switch (renderMode)
	{
	case RenderMode::wireframe:
		glBindVertexArray(m_VAOWireframe);
		glDrawElements(GL_LINES, static_cast<GLsizei>(m_indexCountWireframe), GL_UNSIGNED_INT,
			nullptr);
		break;

	case RenderMode::solid:
		glBindVertexArray(m_VAOSolid);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCountSolid), GL_UNSIGNED_INT,
			nullptr);
		break;
	}
	glBindVertexArray(0);
}

void Mesh::createVBO(const std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
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

void Mesh::createVAOWireframe()
{
	glGenVertexArrays(1, &m_VAOWireframe);

	glBindVertexArray(m_VAOWireframe);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, position)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOWireframe);

	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Mesh::createVAOSolid()
{
	glGenVertexArrays(1, &m_VAOSolid);

	glBindVertexArray(m_VAOSolid);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, normalVector)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOSolid);

	glBindVertexArray(0);
}
