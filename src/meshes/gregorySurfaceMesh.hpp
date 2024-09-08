#pragma once

#include <glm/glm.hpp>

#include <array>
#include <cstddef>

class GregorySurfaceMesh
{
	static constexpr std::size_t m_patchVertexCount = 20;
	static constexpr std::size_t m_vertexCount = 3 * m_patchVertexCount;

public:
	GregorySurfaceMesh(const std::array<glm::vec3, m_vertexCount>& vertices);
	~GregorySurfaceMesh();
	void update(const std::array<glm::vec3, m_vertexCount>& vertices) const;
	void render() const;

private:
	unsigned int m_VBO{};
	unsigned int m_VAO{};
	
	void createVBO(const std::array<glm::vec3, m_vertexCount>& vertices);
	void createVAO();
};
