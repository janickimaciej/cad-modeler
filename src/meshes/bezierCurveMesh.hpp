#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class BezierCurveMesh
{
public:
	BezierCurveMesh(const std::vector<glm::vec3>& vertices,
		const std::vector<unsigned int>& indices);
	~BezierCurveMesh();
	void update(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices);
	void render() const;

private:
	std::size_t m_indexCount{};
	unsigned int m_VBO{};
	unsigned int m_EBO{};
	unsigned int m_VAO{};

	void createVBO(const std::vector<glm::vec3>& vertices);
	void createEBO(const std::vector<unsigned int>& indices);
	void createVAO();

	void updateVBO(const std::vector<glm::vec3>& vertices) const;
	void updateEBO(const std::vector<unsigned int>& indices);
};
