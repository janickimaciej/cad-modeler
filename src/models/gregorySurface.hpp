#pragma once

#include "guis/modelGUIs/gregorySurfaceGUI.hpp"
#include "meshes/mesh.hpp"
#include "models/bezierSurfaces/bezierPatch.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"
#include "shaderPrograms.hpp"

#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <tuple>
#include <vector>

class GregorySurface : public Model
{
public:
	using SelfDestructCallback = std::function<void(const GregorySurface*)>;

	static std::unique_ptr<GregorySurface> create(const std::array<BezierPatch*, 3>& patches,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~GregorySurface() = default;

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;

	bool getRenderVectors() const;
	void setRenderVectors(bool renderVectors);
	int getLineCount() const;
	void setLineCount(int lineCount);

private:
	static int m_count;

	const ShaderProgram& m_surfaceShaderProgram = *ShaderPrograms::gregorySurface;
	const ShaderProgram& m_vectorsShaderProgram = *ShaderPrograms::vectors;

	std::unique_ptr<Mesh> m_surfaceMesh{};
	std::unique_ptr<Mesh> m_vectorsMesh{};
	GregorySurfaceGUI m_gui{*this};

	std::array<std::array<std::array<Point*, 4>, 2>, 3> m_bezierPoints{};
	std::array<std::array<glm::vec3, 20>, 3> m_points{};
	std::vector<std::shared_ptr<Point::MoveCallback>> m_pointMoveNotifications{};
	std::vector<std::shared_ptr<Point::RereferenceCallback>> m_pointRereferenceNotifications{};
	std::vector<Point::DeletabilityLock> m_pointDeletabilityLocks{};
	std::vector<std::shared_ptr<BezierPatch::DestroyCallback>> m_patchDestroyNotifications{};

	bool m_renderVectors = false;
	int m_lineCount = 4;

	SelfDestructCallback m_selfDestructCallback;

	GregorySurface(const std::array<BezierPatch*, 3>& patches,
		const SelfDestructCallback& selfDestructCallback, const std::array<int, 6>& corners);

	void getBezierPoints(const std::array<BezierPatch*, 3>& patches,
		const std::array<int, 6>& corners);
	void createPoints();
	std::vector<glm::vec3> createVectors() const;
	void createSurfaceMesh();
	void createVectorsMesh();
	void updateGeometry();
	void updatePos();
	void updateSurfaceMesh();
	void updateVectorsMesh();

	virtual void updateShaders() const override;
	void renderSurface() const;
	void renderVectors() const;

	void registerForNotifications(const std::array<BezierPatch*, 3>& patches);
	void registerForNotifications(Point* point);
	void pointMoveNotification();
	void pointRereferenceNotification(const Point* point, Point* newPoint);
	void surfaceDestroyNotification();

	static glm::vec3 deCasteljau(const glm::vec3& a, const glm::vec3& b, float t);
	static glm::vec3 deCasteljau(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
		float t);
	static std::optional<std::array<int, 6>> find3Cycle(const std::array<BezierPatch*, 3>& patches);

	std::tuple<int, int, int> getBezierPointIndices(const Point* point) const;
};
