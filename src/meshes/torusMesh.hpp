#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class TorusMesh
{
public:
	struct Vertex
	{
		glm::vec3 pos{};
		glm::vec2 surfacePos{};
	};

	TorusMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	virtual ~TorusMesh();
	void update(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	virtual void render() const;

private:
	std::size_t m_indexCount{};
	unsigned int m_VBO{};
	unsigned int m_EBO{};
	unsigned int m_VAO{};

	void bindVAO() const;
	void unbindVAO() const;
	void createVBO();
	void createEBO();
};
