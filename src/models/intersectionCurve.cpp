#include "models/intersectionCurve.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>

std::unique_ptr<IntersectionCurve> IntersectionCurve::create(const ShaderProgram& shaderProgram,
	const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& cursorPos)
{
	PointPair closestSamples{};
	if (surfaces[0] == surfaces[1])
	{
		closestSamples = findClosestSamples(surfaces[0], cursorPos);
	}
	else
	{
		closestSamples = findClosestSamples(surfaces, cursorPos);
	}
	return create(shaderProgram, surfaces, closestSamples);
}

std::unique_ptr<IntersectionCurve> IntersectionCurve::create(const ShaderProgram& shaderProgram,
	const std::array<const Intersectable*, 2>& surfaces)
{
	PointPair closestSamples{};
	if (surfaces[0] == surfaces[1])
	{
		closestSamples = findClosestSamples(surfaces[0]);
	}
	else
	{
		closestSamples = findClosestSamples(surfaces);
	}
	return create(shaderProgram, surfaces, closestSamples);
}

void IntersectionCurve::render() const
{
	updateShaders();
	m_mesh->render();
}

void IntersectionCurve::updateGUI()
{
	m_gui.update();
}

void IntersectionCurve::setPos(const glm::vec3&)
{ }

void IntersectionCurve::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

int IntersectionCurve::pointCount() const
{
	return static_cast<int>(m_pointPairs.size());
}

int IntersectionCurve::m_count = 0;

std::unique_ptr<IntersectionCurve> IntersectionCurve::create(const ShaderProgram& shaderProgram,
	const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPointPair)
{
	std::optional<PointPair> newtonMethodStartingPointPair = gradientMethod(surfaces,
		startingPointPair);
	if (!newtonMethodStartingPointPair.has_value())
	{
		return nullptr;
	}

	std::vector<PointPair> intersectionPointPair = findIntersectionPoints(surfaces,
		*newtonMethodStartingPointPair);

	return std::unique_ptr<IntersectionCurve>(new IntersectionCurve{shaderProgram, surfaces,
		intersectionPointPair});
}

IntersectionCurve::IntersectionCurve(const ShaderProgram& shaderProgram,
	const std::array<const Intersectable*, 2>& surfaces,
	const std::vector<PointPair>& pointPair) :
	Model{{}, "Intersection curve " + std::to_string(m_count++)},
	m_shaderProgram{shaderProgram},
	m_surfaces{surfaces},
	m_pointPairs{pointPair}
{
	createMesh();
	updatePos();
}

void IntersectionCurve::createMesh()
{
	std::vector<glm::vec3> vertices{};
	for (const PointPair& pointPair : m_pointPairs)
	{
		vertices.push_back(m_surfaces[0]->surface(pointPair[0]));
	}
	m_mesh = std::make_unique<Mesh>(vertices, GL_LINE_STRIP);
}

void IntersectionCurve::updateShaders() const
{
	m_shaderProgram.use();
	m_shaderProgram.setUniform("isDark", false);
	m_shaderProgram.setUniform("isSelected", isSelected());
}

void IntersectionCurve::updatePos()
{
	glm::vec3 pos{};
	for (const PointPair& pointPair : m_pointPairs)
	{
		pos += m_surfaces[0]->surface(pointPair[0]);
	}
	m_pos = pos / static_cast<float>(m_pointPairs.size());
}

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(
	const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& cursorPos)
{
	PointPair closestSamples{};
	for (int i = 0; i < 2; ++i)
	{
		closestSamples[i] = simulatedAnnealing<glm::vec2>(
			[surface = surfaces[i], cursorPos] (const glm::vec2& point)
			{
				return getDistanceSquared(surface->surface(point), cursorPos);
			},
			[surface = surfaces[i]] (const glm::vec2& point)
			{
				return normalizeToDomain(surface, point);
			},
			0.5f,
			10000
		);
	}

	return closestSamples;
}

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(
	const std::array<const Intersectable*, 2>& surfaces)
{
	glm::vec4 closestSamples = simulatedAnnealing<glm::vec4>(
		[surfaces] (const glm::vec4& point)
		{
			PointPair pointPair = vec4ToPointPair(point);
			return getDistanceSquared(surfaces[0]->surface(pointPair[0]),
				surfaces[1]->surface(pointPair[1]));
		},
		[surfaces] (const glm::vec4& point)
		{
			std::optional<PointPair> newPointPair =
				normalizeToDomain(surfaces, vec4ToPointPair(point));
			return pointPairToVec4(newPointPair);
		},
		0.5f,
		10000
	);

	return vec4ToPointPair(closestSamples);
}

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(const Intersectable* surface,
	const glm::vec3& cursorPos)
{
	PointPair closestSamples{};

	closestSamples[0] = simulatedAnnealing<glm::vec2>(
		[surface, cursorPos] (const glm::vec2& point)
		{
			return getDistanceSquared(surface->surface(point), cursorPos);
		},
		[surface] (const glm::vec2& point)
		{
			return normalizeToDomain(surface, point);
		},
		0.5f,
		10000
	);

	closestSamples[1] = simulatedAnnealing<glm::vec2>(
		[surface, firstPoint = closestSamples[0]] (const glm::vec2& point)
		{
			float distanceSquared = getDistanceSquared(surface->surface(firstPoint),
				surface->surface(point));
			float parametersDistanceSquared = getParametersDistanceSquared({firstPoint, point},
				surface->uWrapped(), surface->vWrapped());

			return distanceSquared / parametersDistanceSquared;
		},
		[surface, firstPoint = closestSamples[0]]
		(const glm::vec2& point) -> std::optional<glm::vec2>
		{
			static constexpr float eps = 1e-6f;
			if (getParametersDistanceSquared({firstPoint, point}, surface->uWrapped(),
				surface->vWrapped()) < eps)
			{
				return std::nullopt;
			}

			return normalizeToDomain(surface, point);
		},
		0.5f,
		10000
	);

	return closestSamples;
}

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(const Intersectable* surface)
{
	glm::vec4 closestSamples = simulatedAnnealing<glm::vec4>(
		[surface] (const glm::vec4& point)
		{
			PointPair pointPair = vec4ToPointPair(point);

			float distanceSquared = getDistanceSquared(surface->surface(pointPair[0]),
				surface->surface(pointPair[1]));
			float parametersDistanceSquared = getParametersDistanceSquared(pointPair,
				surface->uWrapped(), surface->vWrapped());

			return distanceSquared / parametersDistanceSquared;
		},
		[surface] (const glm::vec4& point) -> std::optional<glm::vec4>
		{
			PointPair pointPair = vec4ToPointPair(point);

			static constexpr float eps = 1e-6f;
			if (getParametersDistanceSquared(pointPair, surface->uWrapped(),
				surface->vWrapped()) < eps)
			{
				return std::nullopt;
			}

			std::optional<PointPair> newPointPair =
				normalizeToDomain({surface, surface}, pointPair);
			return pointPairToVec4(newPointPair);
		},
		0.5f,
		10000
	);

	return vec4ToPointPair(closestSamples);
}

std::optional<IntersectionCurve::PointPair> IntersectionCurve::gradientMethod(
	const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPointPair)
{
	static constexpr float error = 1e-6f;
	float stepSize = 0.0001f;
	static constexpr std::size_t maxIterations = static_cast<std::size_t>(1e4f);

	PointPair points = startingPointPair;
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
		stepSize *= 0.9999f;
	}
	return std::nullopt;
}

std::vector<IntersectionCurve::PointPair> IntersectionCurve::findIntersectionPoints(
	const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPointPair)
{
	static constexpr float eps = 1.5f * m_newtonMethodStep;
	static constexpr float epsSquared = eps * eps;
	static constexpr std::size_t maxPoints = static_cast<std::size_t>(1e4f);

	std::vector<PointPair> forwardPoints{};
	PointPair pointPair = startingPointPair;
	bool findBackwards = false;
	for (int i = 0; i < maxPoints; ++i)
	{
		if (i > 5 && getDistanceSquared(surfaces[0]->surface(pointPair[0]),
			surfaces[0]->surface(forwardPoints[0][0])) < epsSquared)
		{
			forwardPoints.push_back(forwardPoints[0]);
			return forwardPoints;
		}

		forwardPoints.push_back(pointPair);
		if (outsideDomain(surfaces, pointPair))
		{
			findBackwards = true;
			break;
		}

		std::optional<PointPair> prevPointPair = std::nullopt;
		if (i > 0)
		{
			prevPointPair = forwardPoints[forwardPoints.size() - 2];
		}

		std::optional<PointPair> newPointPair = newtonMethod(surfaces, prevPointPair, pointPair);
		if (!newPointPair.has_value())
		{
			return forwardPoints;
		}
		pointPair = *newPointPair;
	}

	if (!findBackwards)
	{
		return forwardPoints;
	}

	pointPair = startingPointPair;
	std::vector<PointPair> backwardsPoints{};

	for (int i = 0; i < maxPoints - forwardPoints.size(); ++i)
	{
		backwardsPoints.push_back(pointPair);
		if (outsideDomain(surfaces, pointPair))
		{
			break;
		}

		std::optional<PointPair> prevPointPair = std::nullopt;
		if (i > 0)
		{
			prevPointPair = backwardsPoints[backwardsPoints.size() - 2];
		}

		std::optional<PointPair> newPointPair = newtonMethod(surfaces, prevPointPair, pointPair,
			true);
		if (!newPointPair.has_value())
		{
			break;
		}
		pointPair = *newPointPair;
	}

	std::reverse(backwardsPoints.begin(), backwardsPoints.end());
	backwardsPoints.pop_back();
	backwardsPoints.insert(backwardsPoints.end(), forwardPoints.begin(), forwardPoints.end());

	return backwardsPoints;
}

std::optional<IntersectionCurve::PointPair> IntersectionCurve::newtonMethod(
	const std::array<const Intersectable*, 2>& surfaces,
	const std::optional<PointPair>& prevPointPair, const PointPair& startingPointPair,
	bool backwards)
{
	static constexpr float error = 1e-8f;
	static constexpr std::size_t maxIterations = static_cast<std::size_t>(1e4f);

	std::array<glm::vec3, 2> normal{};
	for (std::size_t i = 0; i < 2; ++i)
	{
		normal[i] = glm::cross(surfaces[i]->surfaceDU(startingPointPair[i]),
			surfaces[i]->surfaceDV(startingPointPair[i]));
	}
	glm::vec3 tangent = glm::normalize(glm::cross(normal[0], normal[1]));
	if (backwards)
	{
		tangent *= -1;
	}

	glm::vec3 startingScenePoint = surfaces[0]->surface(startingPointPair[0]);
	if (prevPointPair.has_value())
	{
		glm::vec3 prevScenePoint = surfaces[0]->surface((*prevPointPair)[0]);
		if (glm::dot(tangent, startingScenePoint - prevScenePoint) < 0)
		{
			tangent *= -1;
		}
	}

	glm::vec4 points = {startingPointPair[0], startingPointPair[1]};
	for (std::size_t iteration = 0; iteration < maxIterations; ++iteration)
	{
		std::array<glm::vec3, 2> surface = {surfaces[0]->surface(points.x, points.y),
			surfaces[1]->surface(points.z, points.w)};
		std::array<glm::vec3, 2> surfaceDU = {surfaces[0]->surfaceDU(points.x, points.y),
			surfaces[1]->surfaceDU(points.z, points.w)};
		std::array<glm::vec3, 2> surfaceDV = {surfaces[0]->surfaceDV(points.x, points.y),
			surfaces[1]->surfaceDV(points.z, points.w)};

		glm::vec3 diff = surface[0] - startingScenePoint;
		float tangentDiff = glm::dot(tangent, diff);
		glm::vec4 rhs
		{
			surface[0].x - surface[1].x,
			surface[0].y - surface[1].y,
			surface[0].z - surface[1].z,
			tangentDiff - m_newtonMethodStep
		};

		if (glm::dot(rhs, rhs) < error)
		{
			return {{glm::vec2{points[0], points[1]}, glm::vec2{points[2], points[3]}}};
		}
		float tSurfaceDU = glm::dot(tangent, surfaceDU[0]);
		float tSurfaceDV = glm::dot(tangent, surfaceDV[0]);
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

float IntersectionCurve::getParametersDistanceSquared(const PointPair& pointPair, bool uWrapped,
	bool vWrapped)
{
	float minDistanceSquared = std::numeric_limits<float>::max();
	int uWrap = uWrapped ? 1 : 0;
	int vWrap = vWrapped ? 1 : 0;
	for (int i = -uWrap; i <= uWrap; ++i)
	{
		for (int j = -vWrap; j <= vWrap; ++j)
		{
			glm::vec2 diff = pointPair[1] + glm::vec2{i, j} - pointPair[0];
			float distanceSquared = glm::dot(diff, diff);
			if (distanceSquared < minDistanceSquared)
			{
				minDistanceSquared = distanceSquared;
			}
		}
	}
	return minDistanceSquared;
}

bool IntersectionCurve::outsideDomain(const std::array<const Intersectable*, 2>& surfaces,
	const PointPair& pointPair)
{
	for (int i = 0; i < 2; ++i)
	{
		if (!surfaces[i]->uWrapped() && (pointPair[i].x < 0 || pointPair[i].x > 1) ||
		!surfaces[i]->vWrapped() && (pointPair[i].y < 0 || pointPair[i].y > 1))
		{
			return true;
		}
	}
	return false;
}

std::optional<glm::vec2> IntersectionCurve::normalizeToDomain(const Intersectable* surface,
	const glm::vec2& point)
{
	glm::vec2 newPoint = point;
	if (surface->uWrapped())
	{
		while (newPoint.x < 0)
		{
			newPoint.x += 1;
		}

		while (newPoint.x > 1)
		{
			newPoint.x -= 1;
		}
	}
	else if (newPoint.x < 0 || newPoint.x > 1)
	{
		return std::nullopt;
	}

	if (surface->vWrapped())
	{
		while (newPoint.y < 0)
		{
			newPoint.y += 1;
		}

		while (newPoint.y > 1)
		{
			newPoint.y -= 1;
		}
	}
	else if (newPoint.y < 0 || newPoint.y > 1)
	{
		return std::nullopt;
	}

	return newPoint;
}

std::optional<IntersectionCurve::PointPair> IntersectionCurve::normalizeToDomain(
	const std::array<const Intersectable*, 2>& surfaces, const PointPair& pointPair)
{
	std::array<std::optional<glm::vec2>, 2> newPoints{};
	for (int i = 0; i < 2; ++i)
	{
		newPoints[i] = normalizeToDomain(surfaces[i], pointPair[i]);
		if (!newPoints[i].has_value())
		{
			return std::nullopt;
		}
	}
	return PointPair{*newPoints[0], *newPoints[1]};
}

IntersectionCurve::PointPair IntersectionCurve::vec4ToPointPair(const glm::vec4& vec)
{
	return {glm::vec2{vec[0], vec[1]}, glm::vec2{vec[2], vec[3]}};
}

std::optional<glm::vec4> IntersectionCurve::pointPairToVec4(
	const std::optional<PointPair>& pointPair)
{
	if (!pointPair.has_value())
	{
		return std::nullopt;
	}

	return glm::vec4{(*pointPair)[0].x, (*pointPair)[0].y, (*pointPair)[1].x, (*pointPair)[1].y};
}
