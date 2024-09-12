#pragma once

#include <glm/glm.hpp>

class Intersectable
{
public:
	virtual glm::vec3 surface(float u, float v) const = 0;
	virtual glm::vec3 surfaceDU(float u, float v) const = 0;
	virtual glm::vec3 surfaceDV(float u, float v) const = 0;
};
