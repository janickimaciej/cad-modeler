#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class Mesh
{
public:
	Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices);
	~Mesh();
	void render() const;
	
private:
	std::size_t m_indexCount{};
	unsigned int m_VBO{};
	unsigned int m_EBO{};
	unsigned int m_VAO{};

	void createVBO(const std::vector<glm::vec3>& vertices);
	void createEBO(unsigned int& EBO, const std::vector<unsigned int>& indices);
	void createVAO();
};
