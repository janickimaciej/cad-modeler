#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class Mesh
{
public:
	Mesh(const std::vector<glm::vec3>& vertices, GLenum drawType, GLint patchVertices = 0);
	virtual ~Mesh();
	void update(const std::vector<glm::vec3>& vertices);
	virtual void render() const;

protected:
	void bindVAO() const;
	void unbindVAO() const;
	GLenum drawType() const;
	GLint patchVertices() const;

private:
	std::size_t m_vertexCount{};
	unsigned int m_VBO{};
	unsigned int m_VAO{};
	GLenum m_drawType{};
	GLint m_patchVertices{};

	void createVBO(const std::vector<glm::vec3>& vertices);
};
