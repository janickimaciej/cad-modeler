#pragma once

#include "guis/modelGUIs/intersectionCurveGUI.hpp"
#include "intersectable.hpp"
#include "meshes/mesh.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

class IntersectionCurve : public Model
{
	using PointPair = std::array<glm::vec2, 2>;

public:
	static std::vector<glm::vec3> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& cursorPos);
	static std::vector<glm::vec3> create(const ShaderProgram& shaderProgram,
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
	std::vector<PointPair> m_pointPairs{};
	IntersectionCurveGUI m_gui{*this};

	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPointPair);
	IntersectionCurve(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces,
		const std::vector<PointPair>& pointPair);
	void createMesh();
	virtual void updateShaders() const override;
	void updatePos();

	static PointPair findClosestSamples(const std::array<const Intersectable*, 2>& surfaces,
		const glm::vec3& pos);
	static PointPair findClosestSamples(const std::array<const Intersectable*, 2>& surfaces);
	static PointPair findClosestSamples(const Intersectable* surface, const glm::vec3& pos);
	static PointPair findClosestSamples(const Intersectable* surface);
	static glm::vec4 simulatedAnnealing(const std::function<float(const glm::vec4&)>& function,
		const std::function<bool(const glm::vec4&)>& pointCheck, float startingTemperature,
		int iterations);
	static float intersectionLoss(const std::array<const Intersectable*, 2>& surfaces,
		const PointPair& pointPair);
	static float selfIntersectionLoss(const Intersectable* surface, const PointPair& pointPair);
	static bool intersectionDomainCheck(const std::array<const Intersectable*, 2>& surfaces,
		const PointPair& pointPair);
	static bool selfIntersectionDomainCheck(const Intersectable* surface,
		const PointPair& pointPair);

	static std::optional<PointPair> gradientMethod(
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPointPair);
	static std::vector<PointPair> findIntersectionPoints(
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPointPair);
	static std::optional<PointPair> newtonMethod(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::optional<PointPair>& prevPointPair, const PointPair& startingPointPair,
		bool backwards = false);

	static float getDistanceSquared(const glm::vec2& pos1, const glm::vec2& pos2);
	static float getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2);
	static float getParametersDistanceSquared(const PointPair& pointPair, bool uWrapped,
		bool vWrapped);
	static bool outsideDomain(const std::array<const Intersectable*, 2>& surfaces,
		const PointPair& pointPair);
};
