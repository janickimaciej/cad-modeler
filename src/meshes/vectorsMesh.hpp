#pragma once

#include <glm/glm.hpp>

#include <array>
#include <cstddef>

class VectorsMesh
{
	static constexpr std::size_t m_vertexCount = 48;

public:
	VectorsMesh(const std::array<glm::vec3, m_vertexCount>& vertices);
	~VectorsMesh();
	void update(const std::array<glm::vec3, m_vertexCount>& vertices);
	void render() const;
	
private:
	unsigned int m_VBO{};
	unsigned int m_VAO{};

	void createVBO(const std::array<glm::vec3, m_vertexCount>& vertices);
	void createVAO();

	void updateVBO(const std::array<glm::vec3, m_vertexCount>& vertices) const;
};
