#pragma once

#include "guis/modelGUIs/bezierSurfaceGUI.hpp"
#include "meshes/bezierSurfaceMesh.hpp"
#include "meshes/mesh.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class BezierSurface : public Model
{
public:
	BezierSurface(const std::string& name, const ShaderProgram& bezierSurfaceShaderProgram,
		const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV,
		BezierSurfaceWrapping wrapping);
	virtual ~BezierSurface() = default;
	virtual void render() const override;
	virtual void updateGUI() override;
	
	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	bool getRenderGrid() const;
	void setRenderGrid(bool renderGrid);
	int getLineCount() const;
	void setLineCount(int lineCount);

protected:
	std::unique_ptr<BezierSurfaceMesh> m_surfaceMesh{};
	std::unique_ptr<Mesh> m_gridMesh{};

	const std::size_t m_patchesU{};
	const std::size_t m_patchesV{};
	std::vector<std::vector<Point*>> m_points{};
	std::size_t m_pointsU{};
	std::size_t m_pointsV{};
	BezierSurfaceWrapping m_wrapping{};
	
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
	std::vector<std::vector<glm::vec3>> createIntermediatePoints(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;
	std::vector<std::vector<glm::vec3>> createBezierPoints(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;

	static std::vector<glm::vec3> createVertices(const std::vector<std::vector<Point*>>& points);
	std::vector<unsigned int> createSurfaceIndices() const;
	std::vector<unsigned int> createGridIndices() const;

private:
	const ShaderProgram& m_surfaceShaderProgram;
	const ShaderProgram& m_gridShaderProgram;
	
	BezierSurfaceGUI m_gui{*this};

	bool m_renderGrid = false;
	int m_lineCount = 4;
	
	std::vector<std::shared_ptr<Point::Callback>> m_pointMoveNotifications{};
	
	virtual void updateShaders() const override;
	void renderSurface() const;
	void renderGrid() const;
	
	void registerForNotifications(Point* point);
	void pointMoveNotification();

	std::vector<std::vector<glm::vec3>> createBoorPointsNoWrapping(const glm::vec3& pos,
		float sizeU, float sizeV) const;
	std::vector<std::vector<glm::vec3>> createBoorPointsUWrapping(const glm::vec3& pos,
		float sizeU, float sizeV) const;
	std::vector<std::vector<glm::vec3>> createBoorPointsVWrapping(const glm::vec3& pos,
		float sizeU, float sizeV) const;
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
