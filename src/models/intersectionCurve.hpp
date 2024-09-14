#pragma once

#include "intersectable.hpp"
#include "models/model.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <optional>
#include <vector>

class IntersectionCurve : public Model
{
public:
	static std::vector<std::unique_ptr<IntersectionCurve>> create(
		const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& cursorPos);
	static std::vector<std::unique_ptr<IntersectionCurve>> create(
		const std::array<const Intersectable*, 2>& surfaces);
	virtual ~IntersectionCurve() = default;

private:
	IntersectionCurve();

	static glm::vec2 closestSample(const Intersectable* surface, const glm::vec3& pos);
	static std::optional<std::array<glm::vec2, 2>> gradientMethod(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::array<glm::vec2, 2>& startingPoints);
	static std::vector<std::array<glm::vec2, 2>> findIntersectionPoints(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::array<glm::vec2, 2>& startingPoints);
	static std::optional<std::array<glm::vec2, 2>> newtonMethod(
		const std::array<const Intersectable*, 2>& surfaces,
		const std::array<glm::vec2, 2>& startingPoints);

	static float getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2);
};
