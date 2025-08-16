#pragma once

#include "guis/modelGUIs/bezierSurfaceGUI.hpp"
#include "meshes/indicesMesh.hpp"
#include "models/bezierSurfaces/bezierPatch.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/intersectable.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class BezierSurface : public Intersectable
{
	friend class C0BezierSurfaceSerializer;
	friend class C2BezierSurfaceSerializer;

public:
	using DestroyCallback = std::function<void()>;

	BezierSurface(const Intersectable::ChangeCallback& changeCallback, const std::string& name,
		const ShaderProgram& bezierSurfaceGridShaderProgram, int patchesU, int patchesV,
		BezierSurfaceWrapping wrapping);
	virtual ~BezierSurface();

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;

	bool getRenderGrid() const;
	void setRenderGrid(bool renderGrid);
	int getLineCount() const;
	void setLineCount(int lineCount);

	std::shared_ptr<DestroyCallback> registerForDestroyNotification(
		const DestroyCallback& callback);

	virtual glm::vec3 surface(float u, float v) const override;
	virtual glm::vec3 surfaceDU(float u, float v) const override;
	virtual glm::vec3 surfaceDV(float u, float v) const override;

	virtual bool uWrapped() const override;
	virtual bool vWrapped() const override;

protected:
	std::unique_ptr<IndicesMesh> m_gridMesh{};

	std::vector<std::vector<BezierPatch*>> m_patches{};
	const std::size_t m_patchesU{};
	const std::size_t m_patchesV{};
	std::vector<std::vector<Point*>> m_points{};
	std::size_t m_pointsU{};
	std::size_t m_pointsV{};
	BezierSurfaceWrapping m_wrapping{};

	std::size_t getBezierPointsU() const;
	std::size_t getBezierPointsV() const;

	virtual std::vector<std::unique_ptr<Point>> createPoints(
		const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU,
		float sizeV) = 0;
	std::vector<std::unique_ptr<BezierPatch>> createPatches(
		const ShaderProgram& bezierSurfaceShaderProgram);
	virtual void createGridMesh() = 0;
	virtual void updateGeometry();
	void updatePos();
	void updatePatches();
	virtual void updateGridMesh() = 0;
	virtual std::array<std::array<Point*, 4>, 4> getBezierPoints(std::size_t patchU,
		std::size_t patchV) const = 0;

	void registerForNotifications();

	std::vector<std::vector<glm::vec3>> createBoorPoints(const glm::vec3& pos, float sizeU,
		float sizeV) const;
	std::vector<std::vector<glm::vec3>> createBezierPoints(
		const std::vector<std::vector<glm::vec3>>& boorPoints) const;
	std::vector<unsigned int> createGridIndices() const;

	template <typename PointPtr>
	static std::vector<glm::vec3> createVertices(const std::vector<std::vector<PointPtr>>& points);

private:
	const ShaderProgram& m_gridShaderProgram;

	BezierSurfaceGUI m_gui{*this};

	bool m_renderGrid = false;
	int m_lineCount = 4;

	std::vector<std::weak_ptr<DestroyCallback>> m_destroyNotifications{};

	std::vector<std::shared_ptr<Point::MoveCallback>> m_pointMoveNotifications{};
	std::vector<std::shared_ptr<Point::RereferenceCallback>> m_pointRereferenceNotifications{};
	std::vector<Point::DeletabilityLock> m_pointDeletabilityLocks{};

	virtual void updateShaders() const override;

	void registerForNotifications(Point* point);
	void pointMoveNotification();
	void pointRereferenceNotification(Point* point, Point* newPoint);

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

	void mapToPatch(float u, float v, int& patchU, int& patchV, float& localU, float& localV) const;

	void notifyDestroy();
	void clearExpiredNotifications();

	std::pair<int, int> getPointIndices(const Point* point) const;
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
