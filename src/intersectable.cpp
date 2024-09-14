#include "intersectable.hpp"

glm::vec3 Intersectable::surface(const glm::vec2& pos) const
{
	return surface(pos.x, pos.y);
}

glm::vec3 Intersectable::surfaceDU(const glm::vec2& pos) const
{
	return surfaceDU(pos.x, pos.y);
}

glm::vec3 Intersectable::surfaceDV(const glm::vec2& pos) const
{
	return surfaceDV(pos.x, pos.y);
}
