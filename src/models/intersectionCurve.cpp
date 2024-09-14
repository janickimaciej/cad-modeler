#include "models/intersectionCurve.hpp"

#include <cstddef>
#include <limits>

std::vector<std::unique_ptr<IntersectionCurve>> IntersectionCurve::create(
	const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& cursorPos)
{
	std::array<glm::vec2, 2> gradientMethodStartingPoints{};
	for (std::size_t i = 0; i < 2; ++i)
	{
		gradientMethodStartingPoints[i] = closestSample(surfaces[i], cursorPos);
	}

	std::optional<std::array<glm::vec2, 2>> newtonMethodStartingPoints = gradientMethod(surfaces,
		gradientMethodStartingPoints);
	if (!newtonMethodStartingPoints.has_value())
	{
		return {};
	}
	
	std::vector<std::array<glm::vec2, 2>> intersectionPoints = findIntersectionPoints(surfaces,
		*newtonMethodStartingPoints);

	return {}; // TODO
}

std::vector<std::unique_ptr<IntersectionCurve>> IntersectionCurve::create(
	const std::array<const Intersectable*, 2>& surfaces)
{
	return {}; // TODO
}

glm::vec2 IntersectionCurve::closestSample(const Intersectable* surface, const glm::vec3& pos)
{
	static constexpr int resolution = 10;
	static constexpr float step = 1.0f / resolution;

	float minDistanceSquared = std::numeric_limits<float>::max();
	glm::vec2 closestPos{};
	for (int iu = 0; iu <= resolution; ++iu)
	{
		for (int iv = 0; iv <= resolution; ++iv)
		{
			float u = iu * step;
			float v = iv * step;
			float distanceSquared = getDistanceSquared(surface->surface(u, v), pos);
			if (distanceSquared < minDistanceSquared)
			{
				minDistanceSquared = distanceSquared;
				closestPos = {u, v};
			}
		}
	}
	return closestPos;
}

std::optional<std::array<glm::vec2, 2>> IntersectionCurve::gradientMethod(
	const std::array<const Intersectable*, 2>& surfaces,
	const std::array<glm::vec2, 2>& startingPoints)
{
	static constexpr float error = 1e-5f;
	static constexpr float stepSize = 0.01f;
	static constexpr std::size_t maxIterations = static_cast<std::size_t>(1e4f);

	std::array<glm::vec2, 2> points = startingPoints;
	for (std::size_t iteration = 0; iteration < maxIterations; ++iteration)
	{
		std::array<glm::vec3, 2> surfaceDU = {surfaces[0]->surfaceDU(points[0]),
			surfaces[1]->surfaceDU(points[1])};
		std::array<glm::vec3, 2> surfaceDV = {surfaces[0]->surfaceDV(points[0]),
			surfaces[1]->surfaceDV(points[1])};
		glm::vec3 diff = surfaces[0]->surface(points[0]) - surfaces[1]->surface(points[1]);

		points[0].x -= stepSize * glm::dot(diff, surfaceDU[0]);
		points[0].y -= stepSize * glm::dot(diff, surfaceDV[0]);
		points[1].x += stepSize * glm::dot(diff, surfaceDU[1]);
		points[1].y += stepSize * glm::dot(diff, surfaceDV[1]);

		if (getDistanceSquared(surfaces[0]->surface(points[0]),
			surfaces[1]->surface(points[1])) < error)
		{
			return points;
		}
	}
	return std::nullopt;
}

std::vector<std::array<glm::vec2, 2>> IntersectionCurve::findIntersectionPoints(
	const std::array<const Intersectable*, 2>& surfaces,
	const std::array<glm::vec2, 2>& startingPoints)
{
	std::vector<std::array<glm::vec2, 2>> allPoints{};
	std::array<glm::vec2, 2> points = startingPoints;
	for (int i = 0; i < 100; ++i)
	{
		allPoints.push_back(points);
		std::optional<std::array<glm::vec2, 2>> newPoints = newtonMethod(surfaces, points);
		if (!newPoints.has_value())
		{
			return allPoints;
		}
		points = *newPoints;
	}
	return allPoints;
}

std::optional<std::array<glm::vec2, 2>> IntersectionCurve::newtonMethod(
	const std::array<const Intersectable*, 2>& surfaces,
	const std::array<glm::vec2, 2>& startingPoints)
{
	static constexpr float error = 1e-5f;
	static constexpr std::size_t maxIterations = static_cast<std::size_t>(1e4f);
	static constexpr float d = 0.002f;

	glm::vec3 startingPoint = surfaces[0]->surface(startingPoints[0]);
	glm::vec4 points = {startingPoints[0], startingPoints[1]};
	for (std::size_t iteration = 0; iteration < maxIterations; ++iteration)
	{
		std::array<glm::vec3, 2> surface = {surfaces[0]->surface(points.x, points.y),
			surfaces[1]->surface(points.z, points.w)};
		std::array<glm::vec3, 2> surfaceDU = {surfaces[0]->surfaceDU(points.x, points.y),
			surfaces[1]->surfaceDU(points.z, points.w)};
		std::array<glm::vec3, 2> surfaceDV = {surfaces[0]->surfaceDV(points.x, points.y),
			surfaces[1]->surfaceDV(points.z, points.w)};
		
		std::array<glm::vec3, 2> normal{};
		for (std::size_t i = 0; i < 2; ++i)
		{
			normal[i] = glm::cross(surfaceDU[i], surfaceDV[i]);
		}

		glm::vec3 t = glm::normalize(glm::cross(normal[0], normal[1]));

		glm::vec3 diff = surface[0] - startingPoint;
		float tDiff = glm::dot(t, diff);
		glm::vec4 rhs
		{
			surface[0].x - surface[1].x,
			surface[0].y - surface[1].y,
			surface[0].z - surface[1].z,
			tDiff - d
		};

		if (glm::dot(rhs, rhs) < error)
		{
			return {{glm::vec2{points[0], points[1]}, glm::vec2{points[2], points[3]}}};
		}
		float tSurfaceDU = glm::dot(t, surfaceDU[0]);
		float tSurfaceDV = glm::dot(t, surfaceDV[0]);
		glm::mat4 jacobian
		{
			surfaceDU[0].x, surfaceDU[0].y, surfaceDU[0].z, tSurfaceDU,
			surfaceDV[0].x, surfaceDV[0].y, surfaceDV[0].z, tSurfaceDV,
			-surfaceDU[1].x, -surfaceDU[1].y, -surfaceDU[1].z, 0,
			-surfaceDV[1].x, -surfaceDV[1].y, -surfaceDV[1].z, 0
		};

		glm::mat4 jacobianInverse = glm::inverse(jacobian);
		points -= jacobianInverse * rhs;
	}
	return std::nullopt;
}

float IntersectionCurve::getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2)
{
	glm::vec3 diff = pos2 - pos1;
	return glm::dot(diff, diff);
}
