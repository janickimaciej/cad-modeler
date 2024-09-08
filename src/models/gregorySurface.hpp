#pragma once

#include "guis/modelGUIs/gregorySurfaceGUI.hpp"
#include "meshes/gregorySurfaceMesh.hpp"
#include "meshes/vectorsMesh.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <memory>
#include <vector>

class GregorySurface : public Model
{
public:
	using SelfDestructCallback = std::function<void(const GregorySurface*)>;

	GregorySurface(const ShaderProgram& surfaceShaderProgram,
		const ShaderProgram& vectorsShaderProgram,
		const std::array<std::array<std::array<Point*, 4>, 2>, 3>& bezierPoints,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~GregorySurface() = default;

	virtual void render() const override;
	virtual void updateGUI() override;
	
	bool getRenderVectors() const;
	void setRenderVectors(bool renderVectors);
	int getLineCount() const;
	void setLineCount(int lineCount);

private:
	static int m_count;

	const ShaderProgram& m_surfaceShaderProgram;
	const ShaderProgram& m_vectorsShaderProgram;
	
	std::unique_ptr<GregorySurfaceMesh> m_surfaceMesh{};
	std::unique_ptr<VectorsMesh> m_vectorsMesh{};
	GregorySurfaceGUI m_gui{*this};

	std::array<std::array<std::array<Point*, 4>, 2>, 3> m_bezierPoints{};
	std::array<std::array<glm::vec3, 20>, 3> m_points{};
	std::vector<std::shared_ptr<Point::MoveCallback>> m_pointMoveNotifications{};
	std::vector<std::shared_ptr<Point::DestroyCallback>> m_pointDestroyNotifications{};
	std::vector<std::shared_ptr<Point::RereferenceCallback>> m_pointRereferenceNotifications{};

	bool m_renderVectors = false;
	int m_lineCount = 4;

	SelfDestructCallback m_selfDestructCallback;
	
	void createPoints();
	std::array<glm::vec3, 48> createVectors() const;
	void createSurfaceMesh();
	void createVectorsMesh();
	void updateGeometry();
	void updatePos();
	void updateSurfaceMesh();
	void updateVectorsMesh();

	virtual void updateShaders() const override;
	void renderSurface() const;
	void renderVectors() const;

	void registerForNotifications();
	void registerForNotifications(Point* point);
	void pointMoveNotification();
	void pointDestroyNotification(Point::DestroyCallback*);
	void pointRereferenceNotification(Point::RereferenceCallback* notification, Point* newPoint);

	static glm::vec3 deCasteljau(const glm::vec3& a, const glm::vec3& b, float t);
	static glm::vec3 deCasteljau(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
		float t);
};
