#pragma once

#include "vertex.hpp"

#include <cstddef>
#include <string>
#include <vector>

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	~Mesh();

	void render() const;
	
private:
	std::size_t m_indexCount{};
	unsigned int m_VBO{};
	unsigned int m_EBO{};
	unsigned int m_VAO{};

	void createVBO(const std::vector<Vertex>& vertices);
	void createEBO(unsigned int& EBO, const std::vector<unsigned int>& indices);
	void createVAO();
};
