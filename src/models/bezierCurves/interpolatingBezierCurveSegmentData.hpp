#pragma once

#include <glm/glm.hpp>

#include <vector>

struct InterpolatingBezierCurveSegmentData
{
	glm::vec3 a{};
	glm::vec3 b{};
	glm::vec3 c{};
	glm::vec3 d{};
	glm::vec3 nextPoint{};
	float dt{};
};
