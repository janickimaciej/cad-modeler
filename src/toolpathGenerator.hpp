#pragma once

#include "cameras/orthographicCamera.hpp"
#include "framebuffer.hpp"
#include "quad.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <string>
#include <vector>

class Scene;

class ToolpathGenerator
{
public:
	ToolpathGenerator(const Scene& scene);

	void generatePaths();

private:
	static constexpr glm::ivec2 m_heightmapSize{3000, 3000};

	using TextureData = std::array<std::array<std::array<float, 3>, m_heightmapSize.x>,
		m_heightmapSize.y>;
	using HeightmapData = std::array<std::array<float, m_heightmapSize.x>, m_heightmapSize.y>;

	const Scene& m_scene;

	Framebuffer<float> m_heightmap{GL_FLOAT, GL_RGB32F, m_heightmapSize};
	Framebuffer<float> m_offsetHeightmap{GL_FLOAT, GL_RGB32F, m_heightmapSize};
	Framebuffer<float> m_edge{GL_FLOAT, GL_RGB32F, m_heightmapSize};
	OrthographicCamera m_heightmapCamera;
	Quad m_quad{};

	std::vector<glm::vec3> generateRoughingPath();
	std::vector<glm::vec3> generateFlatPath();
	std::vector<glm::vec3> generateContourPath(float level);
	std::vector<glm::vec3> generateFinishingPath();
	std::vector<glm::vec3> generateIntersectionsPath();

	void generateHeightmap();
	void generateOffsetHeightmap(float radius, bool flatCutter, float pathLevel = 0);
	std::unique_ptr<HeightmapData> getHeightmapData(Framebuffer<float>& heightmap);
	void generateEdge(float level);
	static float getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
		int xIndex, float z);
	static float getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
		float x, float z);
	static float getCurvatureRadius(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	void save(const std::vector<glm::vec3>& path, float yOffset, const std::string& filename);
};
