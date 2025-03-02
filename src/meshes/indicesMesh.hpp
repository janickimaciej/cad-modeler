#pragma once

#include "meshes/mesh.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class IndicesMesh : public Mesh
{
public:
	IndicesMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices,
		GLenum drawType, GLint patchVertices = 0);
	virtual ~IndicesMesh();
	using Mesh::update;
	void update(const std::vector<unsigned int>& indices);
	virtual void render() const override;

private:
	std::size_t m_indexCount{};
	unsigned int m_EBO{};

	void createEBO(const std::vector<unsigned int>& indices);
};
