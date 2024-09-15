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
public:
	static std::vector<std::unique_ptr<IntersectionCurve>> create(
		const ShaderProgram& shaderProgram, const std::array<const Intersectable*, 2>& surfaces,
		const glm::vec3& cursorPos);
	static std::vector<std::unique_ptr<IntersectionCurve>> create(
		const ShaderProgram& shaderProgram, const std::array<const Intersectable*, 2>& surfaces);
	virtual ~IntersectionCurve() = default;

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	int pointCount() const;

private:
	static int m_count;
	
	const ShaderProgram& m_shaderProgram;
	std::unique_ptr<PolylineMesh> m_mesh{};
	std::array<const Intersectable*, 2> m_surfaces{};
	std::vector<std::array<glm::vec2, 2>> m_points{};
	IntersectionCurveGUI m_gui{*this};

	static std::vector<std::unique_ptr<IntersectionCurve>> create(
		const ShaderProgram& shaderProgram, const std::array<const Intersectable*, 2>& surfaces,
		const std::array<glm::vec2, 2>& startingPoints);
	IntersectionCurve(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces,
		const std::vector<std::array<glm::vec2, 2>>& points);
	void createMesh();
	virtual void updateShaders() const override;
	void updatePos();

	static std::array<glm::vec2, 2> closestSamples(
		const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& pos);
	static std::array<glm::vec2, 2> closestSamples(
		const std::array<const Intersectable*, 2>& surfaces);
	static std::optional<std::array<glm::vec2, 2>> gradientMethod(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::array<glm::vec2, 2>& startingPoints);
	static std::vector<std::array<glm::vec2, 2>> findIntersectionPoints(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::array<glm::vec2, 2>& startingPoints);
	static std::optional<std::array<glm::vec2, 2>> newtonMethod(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::array<glm::vec2, 2>& startingPoints, bool backwards = false);

	static float getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2);
	static bool outsideDomain(const std::array<glm::vec2, 2>& points,
		const std::array<const Intersectable*, 2>& surfaces);
};
