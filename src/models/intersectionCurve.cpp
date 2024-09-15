#include "models/intersectionCurve.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <string>

std::vector<std::unique_ptr<IntersectionCurve>> IntersectionCurve::create(
	const ShaderProgram& shaderProgram, const std::array<const Intersectable*, 2>& surfaces,
	const glm::vec3& cursorPos)
{
	return create(shaderProgram, surfaces, closestSamples(surfaces, cursorPos));
}

std::vector<std::unique_ptr<IntersectionCurve>> IntersectionCurve::create(
	const ShaderProgram& shaderProgram, const std::array<const Intersectable*, 2>& surfaces)
{
	return create(shaderProgram, surfaces, closestSamples(surfaces));
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
	return static_cast<int>(m_points.size());
}

int IntersectionCurve::m_count = 0;

std::vector<std::unique_ptr<IntersectionCurve>> IntersectionCurve::create(
	const ShaderProgram& shaderProgram, const std::array<const Intersectable*, 2>& surfaces,
	const std::array<glm::vec2, 2>& startingPoints)
{
	std::optional<std::array<glm::vec2, 2>> newtonMethodStartingPoints = gradientMethod(surfaces,
		startingPoints);
	if (!newtonMethodStartingPoints.has_value())
	{
		return {};
	}

	std::vector<std::array<glm::vec2, 2>> intersectionPoints = findIntersectionPoints(surfaces,
		*newtonMethodStartingPoints);

	std::vector<std::unique_ptr<IntersectionCurve>> curves{};
	curves.push_back(std::unique_ptr<IntersectionCurve>(new IntersectionCurve{shaderProgram,
		surfaces, intersectionPoints}));
	return curves;
}

IntersectionCurve::IntersectionCurve(const ShaderProgram& shaderProgram,
	const std::array<const Intersectable*, 2>& surfaces,
	const std::vector<std::array<glm::vec2, 2>>& points) :
	Model{{}, "Intersection curve " + std::to_string(m_count++)},
	m_shaderProgram{shaderProgram},
	m_surfaces{surfaces},
	m_points{points}
{
	createMesh();
	updatePos();
}

void IntersectionCurve::createMesh()
{
	std::vector<glm::vec3> vertices{};
	for (const std::array<glm::vec2, 2>& point : m_points)
	{
		vertices.push_back(m_surfaces[0]->surface(point[0]));
	}
	m_mesh = std::make_unique<PolylineMesh>(vertices);
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
	for (const std::array<glm::vec2, 2>& point : m_points)
	{
		pos += m_surfaces[0]->surface(point[0]);
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

std::array<glm::vec2, 2> IntersectionCurve::closestSamples(
		const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& pos)
{
	static constexpr int resolution = 10;
	static constexpr float step = 1.0f / resolution;

	std::array<glm::vec2, 2> closestPoints{};
	for (int i = 0; i < 2; ++i)
	{
		float minDistanceSquared = std::numeric_limits<float>::max();
		for (int iu = 0; iu <= resolution; ++iu)
		{
			float u = iu * step;
			for (int iv = 0; iv <= resolution; ++iv)
			{
				float v = iv * step;
				float distanceSquared = getDistanceSquared(surfaces[i]->surface(u, v), pos);
				if (distanceSquared < minDistanceSquared)
				{
					minDistanceSquared = distanceSquared;
					closestPoints[i] = {u, v};
				}
			}
		}
	}
	return closestPoints;
}

std::array<glm::vec2, 2> IntersectionCurve::closestSamples(
	const std::array<const Intersectable*, 2>& surfaces)
{
	static constexpr int resolution = 10;
	static constexpr float step = 1.0f / resolution;

	std::array<glm::vec2, 2> closestPoints{};
	float minDistanceSquared = std::numeric_limits<float>::max();
	for (int iu1 = 0; iu1 <= resolution; ++iu1)
	{
		float u1 = iu1 * step;
		for (int iv1 = 0; iv1 <= resolution; ++iv1)
		{
			float v1 = iv1 * step;
			for (int iu2 = 0; iu2 <= resolution; ++iu2)
			{
				float u2 = iu2 * step;
				for (int iv2 = 0; iv2 <= resolution; ++iv2)
				{
					float v2 = iv2 * step;
					float distanceSquared = getDistanceSquared(surfaces[0]->surface(u1, v1),
						surfaces[1]->surface(u2, v2));
					if (distanceSquared < minDistanceSquared)
					{
						minDistanceSquared = distanceSquared;
						closestPoints[0] = {u1, v1};
						closestPoints[1] = {u2, v2};
					}
				}
			}
		}
	}
	return closestPoints;
}

std::optional<std::array<glm::vec2, 2>> IntersectionCurve::gradientMethod(
	const std::array<const Intersectable*, 2>& surfaces,
	const std::array<glm::vec2, 2>& startingPoints)
{
	static constexpr float error = 1e-8f;
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
	static constexpr float eps = 1.5e-2f * 1.5e-2f;
	static constexpr std::size_t maxPoints = static_cast<std::size_t>(1e4f);

	std::vector<std::array<glm::vec2, 2>> forwardPoints{};
	std::array<glm::vec2, 2> points = startingPoints;
	bool findBackwards = false;
	for (int i = 0; i < maxPoints; ++i)
	{
		if (i > 5 && getDistanceSquared(surfaces[0]->surface(points[0]),
			surfaces[0]->surface(forwardPoints[0][0])) < eps)
		{
			forwardPoints.push_back(forwardPoints[0]);
			return forwardPoints;
		}

		forwardPoints.push_back(points);
		if (outsideDomain(points, surfaces))
		{
			findBackwards = true;
			break;
		}

		std::optional<std::array<glm::vec2, 2>> newPoints = newtonMethod(surfaces, points);
		if (!newPoints.has_value())
		{
			break;
		}
		points = *newPoints;
	}

	if (!findBackwards)
	{
		return forwardPoints;
	}

	points = startingPoints;
	std::vector<std::array<glm::vec2, 2>> backwardsPoints{};

	for (int i = 0; i < maxPoints - forwardPoints.size(); ++i)
	{
		std::optional<std::array<glm::vec2, 2>> newPoints = newtonMethod(surfaces, points, true);
		if (!newPoints.has_value())
		{
			break;
		}
		points = *newPoints;

		backwardsPoints.push_back(points);
		if (outsideDomain(points, surfaces))
		{
			break;
		}
	}

	std::reverse(backwardsPoints.begin(), backwardsPoints.end());
	backwardsPoints.insert(backwardsPoints.end(), forwardPoints.begin(), forwardPoints.end());

	return backwardsPoints;
}

std::optional<std::array<glm::vec2, 2>> IntersectionCurve::newtonMethod(
	const std::array<const Intersectable*, 2>& surfaces,
	const std::array<glm::vec2, 2>& startingPoints, bool backwards)
{
	static constexpr float error = 1e-8f;
	static constexpr std::size_t maxIterations = static_cast<std::size_t>(1e4f);
	static constexpr float d = 0.01f;

	std::array<glm::vec3, 2> normal{};
	for (std::size_t i = 0; i < 2; ++i)
	{
		normal[i] = glm::cross(surfaces[i]->surfaceDU(startingPoints[i]),
			surfaces[i]->surfaceDV(startingPoints[i]));
	}
	glm::vec3 t = glm::normalize(glm::cross(normal[0], normal[1]));

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

		glm::vec3 diff = surface[0] - startingPoint;
		float tDiff = glm::dot(t, diff);
		glm::vec4 rhs
		{
			surface[0].x - surface[1].x,
			surface[0].y - surface[1].y,
			surface[0].z - surface[1].z,
			tDiff - (backwards ? -d : d)
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

bool IntersectionCurve::outsideDomain(const std::array<glm::vec2, 2>& points,
	const std::array<const Intersectable*, 2>& surfaces)
{
	return !surfaces[0]->uWrapped() && (points[0].x < 0 || points[0].x > 1) ||
		!surfaces[0]->vWrapped() && (points[0].y < 0 || points[0].y > 1) ||
		!surfaces[1]->uWrapped() && (points[1].x < 0 || points[1].x > 1) ||
		!surfaces[1]->vWrapped() && (points[1].y < 0 || points[1].y > 1);
}
