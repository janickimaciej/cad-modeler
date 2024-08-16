#pragma once

#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class BezierSurfaceC0 : public Model
{
	BezierSurfaceC0(const ShaderProgram& bezierSurfaceShaderProgram,
		const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV,
		std::vector<std::unique_ptr<Point>>& bezierPoints);
	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	bool getRenderGrid() const;
	void setRenderGrid(bool renderGrid);

private:
	static int m_count;

	const ShaderProgram& m_surfaceShaderProgram;
	const ShaderProgram& m_gridShaderProgram;
	
	// std::unique_ptr<BezierSurfaceMesh> m_surfaceMesh{};
	// std::unique_ptr<BezierSurfaceGridMesh> m_gridMesh{};
	// BezierSurfaceGUI m_gui{*this};

	std::vector<Point*> m_points{};
	bool m_renderGrid = true;
	
	std::vector<std::shared_ptr<Point::Callback>> m_pointMoveNotifications{};
	
	virtual void updateShaders() const override;

	void createSurfaceMesh();
	std::vector<std::unique_ptr<Point>> createBezierPoints();
	void createGridMesh();
	
	virtual void updateGeometry();
	void updatePos();
	void updateSurfaceMesh();
	void updateGridMesh();

	void renderSurface() const;
	void renderGrid() const;
	
	void registerForNotifications(const std::vector<Point*>& points);
	void registerForNotifications(Point* point);
	void pointMoveNotification();

	static std::vector<glm::vec3> pointsToVertices(const std::vector<Point*> points);
	static std::vector<unsigned int> pointsToCurveIndices(const std::vector<Point*> points);
};
