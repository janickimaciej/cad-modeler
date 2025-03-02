#pragma once

#include "guis/modelGUIs/bezierPatchGUI.hpp"
#include "intersectable.hpp"
#include "meshes/mesh.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <vector>

class BezierSurface;

class BezierPatch : public Model, public Intersectable
{
public:
	using DestroyCallback = std::function<void()>;

	BezierPatch(const ShaderProgram& shaderProgram,
		const std::array<std::array<Point*, 4>, 4>& bezierPoints,
		const BezierSurface& surface, bool isOnNegativeUEdge, bool isOnPositiveUEdge,
		bool isOnNegativeVEdge, bool isOnPositiveVEdge);
	virtual ~BezierPatch();
	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;
	void updatePoints(const std::array<std::array<Point*, 4>, 4>& bezierPoints);

	bool isInvalid() const;
	void setInvalid();

	Point* getCornerPointIfOnEdge(int corner) const;
	std::array<std::array<Point*, 4>, 2> getPointsBetweenCorners(int leftCorner,
		int rightCorner) const;

	std::shared_ptr<DestroyCallback> registerForDestroyNotification(
		const DestroyCallback& callback);

	virtual glm::vec3 surface(float u, float v) const override;
	virtual glm::vec3 surfaceDU(float u, float v) const override;
	virtual glm::vec3 surfaceDV(float u, float v) const override;

	virtual bool uWrapped() const override;
	virtual bool vWrapped() const override;

private:
	static int m_count;

	const ShaderProgram& m_shaderProgram;
	std::unique_ptr<Mesh> m_mesh{};
	BezierPatchGUI m_gui{*this};

	std::array<std::array<Point*, 4>, 4> m_bezierPoints{};

	const BezierSurface& m_surface;
	bool m_isInvalid = false;

	bool m_isOnNegativeUEdge{};
	bool m_isOnPositiveUEdge{};
	bool m_isOnNegativeVEdge{};
	bool m_isOnPositiveVEdge{};

	std::vector<std::weak_ptr<DestroyCallback>> m_destroyNotifications{};

	void createSurfaceMesh();
	void updatePos();
	void updateSurfaceMesh();
	std::vector<glm::vec3> createVertices();

	virtual void updateShaders() const override;

	void notifyDestroy();
	void clearExpiredNotifications();

	static glm::vec3 deCasteljau(const glm::vec3& a, const glm::vec3& b, float t);
	static glm::vec3 deCasteljau(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
		float t);
	static glm::vec3 deCasteljau(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
		const glm::vec3& d,	float t);
	static glm::vec3 deCasteljauDT(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
		const glm::vec3& d, float t);
};
