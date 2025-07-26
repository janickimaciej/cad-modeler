#pragma once

#include "guis/modelGUIs/intersectionCurveGUI.hpp"
#include "intersectable.hpp"
#include "meshes/mesh.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <optional>
#include <vector>

class IntersectionCurve : public Model
{
	using PointPair = std::array<glm::vec2, 2>;

public:
	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& cursorPos);
	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces);
	virtual ~IntersectionCurve() = default;

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	int pointCount() const;

private:
	static constexpr float m_newtonMethodStep = 0.01f;

	static int m_count;

	const ShaderProgram& m_shaderProgram;
	std::unique_ptr<Mesh> m_mesh{};
	std::array<const Intersectable*, 2> m_surfaces{};
	std::vector<PointPair> m_points{};
	IntersectionCurveGUI m_gui{*this};

	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPoint);
	IntersectionCurve(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces,
		const std::vector<PointPair>& points);
	void createMesh();
	virtual void updateShaders() const override;
	void updatePos();

	static PointPair findClosestSamples(const std::array<const Intersectable*, 2>& surfaces,
		const glm::vec3& pos);
	static PointPair findClosestSamples(const std::array<const Intersectable*, 2>& surfaces);
	static PointPair findClosestSamples(const Intersectable* surface, const glm::vec3& pos);
	static PointPair findClosestSamples(const Intersectable* surface);
	static std::optional<PointPair> gradientMethod(
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPoints);
	static std::vector<PointPair> findIntersectionPoints(
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPoint);
	static std::optional<PointPair> newtonMethod(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::optional<PointPair>& prevPoint, const PointPair& startingPoint,
		bool backwards = false);

	static float getDistanceSquared(const glm::vec2& pos1, const glm::vec2& pos2);
	static float getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2);
	static bool outsideDomain(const PointPair& point,
		const std::array<const Intersectable*, 2>& surfaces);
};
