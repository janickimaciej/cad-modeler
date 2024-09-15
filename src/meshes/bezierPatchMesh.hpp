#pragma once

#include <glm/glm.hpp>

#include <array>

class BezierPatchMesh
{
public:
	BezierPatchMesh(const std::array<glm::vec3, 16>& vertices);
	~BezierPatchMesh();
	void update(const std::array<glm::vec3, 16>& vertices) const;
	void render() const;

private:
	unsigned int m_VBO{};
	unsigned int m_VAO{};

	void createVBO(const std::array<glm::vec3, 16>& vertices);
	void createVAOs();
};
