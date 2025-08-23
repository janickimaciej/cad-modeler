#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class FlatMesh
{
public:
	FlatMesh(const std::vector<glm::vec2>& vertices, const std::vector<unsigned int>& indices);
	virtual ~FlatMesh();
	virtual void render() const;

private:
	std::size_t m_indexCount{};
	unsigned int m_VBO{};
	unsigned int m_EBO{};
	unsigned int m_VAO{};

	void bindVAO() const;
	void unbindVAO() const;
	void createVBO(const std::vector<glm::vec2>& vertices);
	void createEBO(const std::vector<unsigned int>& indices);
};
