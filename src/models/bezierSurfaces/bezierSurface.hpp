#pragma once

#include "guis/modelGUIs/bezierSurfaceGUI.hpp"
#include "meshes/bezierSurfaceMesh.hpp"
#include "meshes/mesh.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class BezierSurface : public Model
{
	friend class BezierSurfaceC0Serializer;
	friend class BezierSurfaceC2Serializer;

public:
	BezierSurface(const std::string& name, const ShaderProgram& bezierSurfaceShaderProgram,
		const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV,
		BezierSurfaceWrapping wrapping);
	virtual ~BezierSurface();
	virtual void render() const override;
	virtual void updateGUI() override;
	
	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	bool getRenderGrid() const;
	void setRenderGrid(bool renderGrid);
	int getLineCount() const;
	void setLineCount(int lineCount);
	std::size_t getPatchCount() const;
	void selectPatch(std::size_t patch);
	void deselectPatch();
	virtual Point* getCornerPointIfOnEdge(std::size_t patch, int corner) = 0;
	virtual std::array<std::array<Point*, 4>, 2> getPointsBetweenCorners(std::size_t patch,
		int leftCorner, int rightCorner) = 0;

protected:
	std::unique_ptr<BezierSurfaceMesh> m_surfaceMesh{};
	std::unique_ptr<Mesh> m_gridMesh{};

	const std::size_t m_patchesU{};
	const std::size_t m_patchesV{};
	std::optional<std::size_t> m_selectedPatch = std::nullopt;
	std::vector<std::vector<Point*>> m_points{};
	std::size_t m_pointsU{};
	std::size_t m_pointsV{};
	BezierSurfaceWrapping m_wrapping{};

	std::size_t getBezierPointsU() const;
	std::size_t getBezierPointsV() const;
	
	virtual std::vector<std::unique_ptr<Point>> createPoints(
		const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU,
		float sizeV) = 0;
	virtual void createSurfaceMesh() = 0;
	virtual void createGridMesh() = 0;
	virtual void updateGeometry();
	void updatePos();
	virtual void updateSurfaceMesh() = 0;
	virtual void updateGridMesh() = 0;

	void registerForNotifications();

	std::vector<std::vector<glm::vec3>> createBoorPoints(const glm::vec3& pos, float sizeU,
		float sizeV) const;
	std::vector<std::vector<glm::vec3>> createBezierPoints(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;

	std::vector<std::vector<unsigned int>> createSurfaceIndices() const;
	std::vector<unsigned int> createGridIndices() const;

	template <typename PointPtr>
	static std::vector<glm::vec3> createVertices(const std::vector<std::vector<PointPtr>>& points);

	template <typename PointPtr>
	std::array<std::array<Point*, 4>, 2> getPointsBetweenCorners(
		const std::vector<std::vector<PointPtr>>& bezierPoints, std::size_t patch, int leftCorner,
		int rightCorner);

private:
	const ShaderProgram& m_surfaceShaderProgram;
	const ShaderProgram& m_gridShaderProgram;
	
	BezierSurfaceGUI m_gui{*this};

	bool m_renderGrid = false;
	int m_lineCount = 4;
	
	std::vector<std::shared_ptr<Point::MoveCallback>> m_pointMoveNotifications{};
	std::vector<std::shared_ptr<Point::RereferenceCallback>> m_pointRereferenceNotifications{};
	std::vector<Point::DeletabilityLock> m_pointDeletabilityLocks{};
	
	virtual void updateShaders() const override;
	void renderSurface() const;
	void renderGrid() const;
	
	void registerForNotifications(Point* point);
	void pointMoveNotification();
	void pointRereferenceNotification(Point::RereferenceCallback* notification, Point* newPoint);

	std::vector<std::vector<glm::vec3>> createBoorPointsNoWrapping(const glm::vec3& pos,
		float sizeU, float sizeV) const;
	std::vector<std::vector<glm::vec3>> createBoorPointsUWrapping(const glm::vec3& pos,
		float sizeU, float sizeV) const;
	std::vector<std::vector<glm::vec3>> createBoorPointsVWrapping(const glm::vec3& pos,
		float sizeU, float sizeV) const;
	std::vector<std::vector<glm::vec3>> createIntermediatePoints(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;
	std::vector<std::vector<glm::vec3>> createIntermediatePointsNoWrapping(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;
	std::vector<std::vector<glm::vec3>> createIntermediatePointsUWrapping(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;
	std::vector<std::vector<glm::vec3>> createIntermediatePointsVWrapping(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;
	void createBezierPointsNoWrapping(const std::vector<std::vector<glm::vec3>>& boorPoints,
		std::vector<std::vector<glm::vec3>>& bezierPoints) const;
	void createBezierPointsUWrapping(const std::vector<std::vector<glm::vec3>>& boorPoints,
		std::vector<std::vector<glm::vec3>>& bezierPoints) const;
	void createBezierPointsVWrapping(const std::vector<std::vector<glm::vec3>>& boorPoints,
		std::vector<std::vector<glm::vec3>>& bezierPoints) const;

	std::vector<unsigned int> createGridIndicesNoWrapping() const;
	std::vector<unsigned int> createGridIndicesUWrapping() const;
	std::vector<unsigned int> createGridIndicesVWrapping() const;
};

template <typename PointPtr>
std::vector<glm::vec3> BezierSurface::createVertices(
	const std::vector<std::vector<PointPtr>>& points)
{
	std::vector<glm::vec3> vertices{};
	for (const std::vector<PointPtr>& row : points)
	{
		for (const PointPtr& point : row)
		{
			vertices.push_back(point->getPos());
		}
	}
	return vertices;
}

template <typename PointPtr>
std::array<std::array<Point*, 4>, 2> BezierSurface::getPointsBetweenCorners(
	const std::vector<std::vector<PointPtr>>& bezierPoints, std::size_t patch, int leftCorner,
	int rightCorner)
{
	std::size_t patchU = patch % m_patchesU;
	std::size_t patchV = patch / m_patchesU;
	std::size_t startingCornerU = 3 * patchU;
	std::size_t startingCornerV = 3 * patchV;
	std::size_t endingCornerU = 3 * patchU + 3;
	std::size_t endingCornerV = 3 * patchV + 3;

	std::array<std::array<Point*, 4>, 2> points{};
	for (std::size_t i = 0; i < 2; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			std::size_t u{};
			std::size_t v{};
			switch (leftCorner)
			{
				case 0:
					if (rightCorner == 3)
					{
						u = startingCornerU + i;
						v = startingCornerV + j;
					}
					else
					{
						u = startingCornerU + j;
						v = startingCornerV + i;
					}
					break;

				case 1:
					if (rightCorner == 0)
					{
						u = endingCornerU - j;
						v = startingCornerV + i;
					}
					else
					{
						u = endingCornerU - i;
						v = startingCornerV + j;
					}
					break;

				case 2:
					if (rightCorner == 1)
					{
						u = endingCornerU - i;
						v = endingCornerV - j;
					}
					else
					{
						u = endingCornerU - j;
						v = endingCornerV - i;
					}
					break;

				case 3:
					if (rightCorner == 2)
					{
						u = startingCornerU + j;
						v = endingCornerV - i;
					}
					else
					{
						u = startingCornerU + i;
						v = endingCornerV - j;
					}
					break;
			}
			points[i][j] = &*bezierPoints[v % m_pointsV][u % m_pointsU];
		}
	}
	return points;
}
