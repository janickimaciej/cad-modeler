#include "meshes/indicesMesh.hpp"

IndicesMesh::IndicesMesh(const std::vector<glm::vec3>& vertices,
	const std::vector<unsigned int>& indices, GLenum drawType, GLint patchVertices) :
	Mesh{vertices, drawType, patchVertices}
{
	createEBO(indices);
}

IndicesMesh::~IndicesMesh()
{
	glDeleteBuffers(1, &m_EBO);
}

void IndicesMesh::update(const std::vector<unsigned int>& indices)
{
	m_indexCount = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
		GL_DYNAMIC_DRAW);
}

void IndicesMesh::render() const
{
	if (drawType() == GL_PATCHES)
	{
		glPatchParameteri(GL_PATCH_VERTICES, patchVertices());
	}

	bindVAO();
	glDrawElements(drawType(), static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
	unbindVAO();
}

void IndicesMesh::createEBO(const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &m_EBO);

	bindVAO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	unbindVAO();

	update(indices);
}
