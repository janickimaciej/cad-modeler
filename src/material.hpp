#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::ivec3 color{};
	float ambientCoef{};
	float diffuseCoef{};
	float specularCoef{};
	float shininess{};

	Material(const glm::ivec3& color, float ambientCoef, float diffuseCoef, float specularCoef,
		float shininess);
};
