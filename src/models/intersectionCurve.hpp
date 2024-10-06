#pragma once

#include "guis/modelGUIs/intersectionCurveGUI.hpp"
#include "intersectable.hpp"
#include "meshes/polylineMesh.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <optional>
#include <vector>

class IntersectionCurve : public Model
{
	using ParametersPoint = std::array<glm::vec2, 2>;

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
	std::unique_ptr<PolylineMesh> m_mesh{};
	std::array<const Intersectable*, 2> m_surfaces{};
	std::vector<ParametersPoint> m_points{};
	IntersectionCurveGUI m_gui{*this};

	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces,
		const ParametersPoint& startingPoint);
	IntersectionCurve(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces,
		const std::vector<ParametersPoint>& points);
	void createMesh();
	virtual void updateShaders() const override;
	void updatePos();

	static ParametersPoint closestSamples(
		const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& pos);
	static ParametersPoint closestSamples(const std::array<const Intersectable*, 2>& surfaces);
	static std::optional<ParametersPoint> gradientMethod(
		const std::array<const Intersectable*, 2>& surfaces, const ParametersPoint& startingPoints);
	static std::vector<ParametersPoint> findIntersectionPoints(
		const std::array<const Intersectable*, 2>& surfaces, const ParametersPoint& startingPoint);
	static std::optional<ParametersPoint> newtonMethod(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::optional<ParametersPoint>& prevPoint, const ParametersPoint& startingPoint,
		bool backwards = false);

	static float getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2);
	static bool outsideDomain(const ParametersPoint& point,
		const std::array<const Intersectable*, 2>& surfaces);
};
