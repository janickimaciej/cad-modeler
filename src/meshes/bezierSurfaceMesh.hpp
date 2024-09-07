#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class BezierSurfaceMesh
{
public:
	BezierSurfaceMesh(const std::vector<glm::vec3>& vertices,
		const std::vector<std::vector<unsigned int>>& indices);
	~BezierSurfaceMesh();
	void update(const std::vector<glm::vec3>& vertices,
		const std::vector<std::vector<unsigned int>>& indices);
	void render(std::size_t patch) const;

private:
	std::size_t m_patchCount{};
	unsigned int m_VBO{};
	std::vector<unsigned int> m_EBOs{};
	std::vector<unsigned int> m_VAOs{};
	
	void createVBO(const std::vector<glm::vec3>& vertices);
	void createEBOs(const std::vector<std::vector<unsigned int>>& indices);
	void createVAOs();

	void updateVBO(const std::vector<glm::vec3>& vertices) const;
	void updateEBOs(const std::vector<std::vector<unsigned int>>& indices);
};
