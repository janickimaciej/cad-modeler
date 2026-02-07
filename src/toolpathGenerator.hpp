#pragma once

#include "cameras/orthographicCamera.hpp"
#include "heightmap.hpp"
#include "quad.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <string>
#include <vector>

class Scene;

static inline constexpr glm::ivec2 heightmapSize{3000, 3000};

class ToolpathGenerator
{
public:
	ToolpathGenerator(const Scene& scene);

	void generatePaths();

private:
	using TextureData = std::array<std::array<std::array<float, 3>, heightmapSize.x>,
		heightmapSize.y>;
	using HeightmapData = std::array<std::array<float, heightmapSize.x>, heightmapSize.y>;

	const Scene& m_scene;

	Heightmap m_heightmap{heightmapSize};
	Heightmap m_offsetHeightmap{heightmapSize};
	Heightmap m_edge{heightmapSize};
	OrthographicCamera m_heightmapCamera;
	Quad m_quad{};

	std::vector<glm::vec3> generateRoughingPath();
	std::vector<glm::vec3> generateFlatPath();
	std::vector<glm::vec3> generateContourPath(float level);
	std::vector<glm::vec3> generateFinishingPath();
	std::vector<glm::vec3> generateIntersectionsPath();

	void generateHeightmap();
	void generateOffsetHeightmap(float radius, bool flatCutter, float pathLevel = 0);
	std::unique_ptr<HeightmapData> getHeightmapData(Heightmap& heightmap);
	void generateEdge(float level);
	static float getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
		int xIndex, float z);
	static float getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
		float x, float z);
	static float getCurvatureRadius(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	void savePath(const std::vector<glm::vec3>& path, float yOffset, const std::string& filename);
};
