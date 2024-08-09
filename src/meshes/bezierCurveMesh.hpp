#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class BezierCurveMesh
{
public:
	BezierCurveMesh(const std::vector<glm::vec3>& vertices);
	~BezierCurveMesh();
	void update(const std::vector<glm::vec3>& vertices);
	void render() const;

private:
	std::size_t m_vertexCount{};
	unsigned int m_VBO{};
	unsigned int m_VAO{};
	
	void createVBO(const std::vector<glm::vec3>& vertices);
	void createVAO();
};
