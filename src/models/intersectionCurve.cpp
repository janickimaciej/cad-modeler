#include "models/intersectionCurve.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <cstddef>
#include <limits>
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
	return static_cast<int>(m_points.size());
}

int IntersectionCurve::m_count = 0;

std::unique_ptr<IntersectionCurve> IntersectionCurve::create(const ShaderProgram& shaderProgram,
	const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPoint)
{
	std::optional<PointPair> newtonMethodStartingPoint = gradientMethod(surfaces,
		startingPoint);
	if (!newtonMethodStartingPoint.has_value())
	{
		return nullptr;
	}

	std::vector<PointPair> intersectionPoints = findIntersectionPoints(surfaces,
		*newtonMethodStartingPoint);

	return std::unique_ptr<IntersectionCurve>(new IntersectionCurve{shaderProgram, surfaces,
		intersectionPoints});
}

IntersectionCurve::IntersectionCurve(const ShaderProgram& shaderProgram,
	const std::array<const Intersectable*, 2>& surfaces,
	const std::vector<PointPair>& points) :
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
	for (const PointPair& point : m_points)
	{
		vertices.push_back(m_surfaces[0]->surface(point[0]));
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
	for (const PointPair& point : m_points)
	{
		pos += m_surfaces[0]->surface(point[0]);
	}
	m_pos = pos / static_cast<float>(m_points.size());
}

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(
	const std::array<const Intersectable*, 2>& surfaces, const glm::vec3& pos)
{
	static constexpr int resolution = 30;
	static constexpr float step = 1.0f / resolution;

	PointPair closestPoints{};

	for (int i = 0; i < 2; ++i)
	{
		int maxIU = surfaces[i]->uWrapped() ? resolution - 1 : resolution;
		int maxIV = surfaces[i]->vWrapped() ? resolution - 1 : resolution;
		float minDistanceSquared = std::numeric_limits<float>::max();
		for (int iu = 0; iu <= maxIU; ++iu)
		{
			float u = iu * step;
			for (int iv = 0; iv <= maxIV; ++iv)
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

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(
	const std::array<const Intersectable*, 2>& surfaces)
{
	static constexpr int resolution = 30;
	static constexpr float step = 1.0f / resolution;

	PointPair closestPoints{};
	float minDistanceSquared = std::numeric_limits<float>::max();

	int maxIU1 = surfaces[0]->uWrapped() ? resolution - 1 : resolution;
	int maxIV1 = surfaces[0]->vWrapped() ? resolution - 1 : resolution;
	int maxIU2 = surfaces[1]->uWrapped() ? resolution - 1 : resolution;
	int maxIV2 = surfaces[1]->vWrapped() ? resolution - 1 : resolution;
	for (int iu1 = 0; iu1 <= maxIU1; ++iu1)
	{
		float u1 = iu1 * step;
		for (int iv1 = 0; iv1 <= maxIV1; ++iv1)
		{
			float v1 = iv1 * step;
			for (int iu2 = 0; iu2 <= maxIU2; ++iu2)
			{
				float u2 = iu2 * step;
				for (int iv2 = 0; iv2 <= maxIV2; ++iv2)
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

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(const Intersectable* surface,
	const glm::vec3& pos)
{
	static constexpr int resolution = 30;
	static constexpr float step = 1.0f / resolution;

	PointPair closestPoints{};
	float minDistanceSquared = std::numeric_limits<float>::max();
	float secondMinDistanceSquared = std::numeric_limits<float>::max();

	int maxIU = surface->uWrapped() ? resolution - 1 : resolution;
	int maxIV = surface->vWrapped() ? resolution - 1 : resolution;
	for (int iu = 0; iu <= maxIU; ++iu)
	{
		float u = iu * step;
		for (int iv = 0; iv <= maxIV; ++iv)
		{
			float v = iv * step;
			float distanceSquared = getDistanceSquared(surface->surface(u, v), pos);
			if (distanceSquared < minDistanceSquared)
			{
				secondMinDistanceSquared = minDistanceSquared;
				closestPoints[1] = closestPoints[0];
				minDistanceSquared = distanceSquared;
				closestPoints[0] = {u, v};
			}
			else if (distanceSquared < secondMinDistanceSquared)
			{
				secondMinDistanceSquared = distanceSquared;
				closestPoints[1] = {u, v};
			}
		}
	}
	return closestPoints;
}

IntersectionCurve::PointPair IntersectionCurve::findClosestSamples(const Intersectable* surface)
{
	static constexpr int resolution = 30;
	static constexpr float step = 1.0f / resolution;

	PointPair closestPoints{};
	float minLoss = std::numeric_limits<float>::max();

	int maxIU = surface->uWrapped() ? resolution - 1 : resolution;
	int maxIV = surface->vWrapped() ? resolution - 1 : resolution;
	for (int iu1 = 0; iu1 <= maxIU; ++iu1)
	{
		float u1 = iu1 * step;
		for (int iv1 = 0; iv1 <= maxIV; ++iv1)
		{
			float v1 = iv1 * step;
			for (int iu2 = 0; iu2 <= maxIU; ++iu2)
			{
				float u2 = iu2 * step;
				for (int iv2 = 0; iv2 <= maxIV; ++iv2)
				{
					float v2 = iv2 * step;

					if (iv1 == iv2 && iu1 == iu2)
					{
						continue;
					}

					float distanceSquared = getDistanceSquared(surface->surface(u1, v1),
						surface->surface(u2, v2));
					float minParametersDistanceSquared = std::numeric_limits<float>::max();
					int uWrap = surface->uWrapped() ? 1 : 0;
					int vWrap = surface->vWrapped() ? 1 : 0;
					for (int i = -uWrap; i <= uWrap; ++i)
					{
						for (int j = -vWrap; j <= vWrap; ++j)
						{
							float parametersDistanceSquared =
								getDistanceSquared({u1, v1}, {u2 + i, v2 + j});
							if (parametersDistanceSquared < minParametersDistanceSquared)
							{
								minParametersDistanceSquared = parametersDistanceSquared;
							}
						}
					}
					float loss = distanceSquared - minParametersDistanceSquared;
					if (loss < minLoss)
					{
						minLoss = loss;
						closestPoints[0] = {u1, v1};
						closestPoints[1] = {u2, v2};
					}
				}
			}
		}
	}
	return closestPoints;
}

std::optional<IntersectionCurve::PointPair> IntersectionCurve::gradientMethod(
	const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPoints)
{
	static constexpr float error = 1e-6f;
	float stepSize = 0.0001f;
	static constexpr std::size_t maxIterations = static_cast<std::size_t>(1e4f);

	PointPair points = startingPoints;
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
	const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPoint)
{
	static constexpr float eps = 1.5f * m_newtonMethodStep;
	static constexpr float epsSquared = eps * eps;
	static constexpr std::size_t maxPoints = static_cast<std::size_t>(1e4f);

	std::vector<PointPair> forwardPoints{};
	PointPair point = startingPoint;
	bool findBackwards = false;
	for (int i = 0; i < maxPoints; ++i)
	{
		if (i > 5 && getDistanceSquared(surfaces[0]->surface(point[0]),
			surfaces[0]->surface(forwardPoints[0][0])) < epsSquared)
		{
			forwardPoints.push_back(forwardPoints[0]);
			return forwardPoints;
		}

		forwardPoints.push_back(point);
		if (outsideDomain(point, surfaces))
		{
			findBackwards = true;
			break;
		}

		std::optional<PointPair> prevPoint = std::nullopt;
		if (i > 0)
		{
			prevPoint = forwardPoints[forwardPoints.size() - 2];
		}

		std::optional<PointPair> newPoint = newtonMethod(surfaces, prevPoint, point);
		if (!newPoint.has_value())
		{
			return forwardPoints;
		}
		point = *newPoint;
	}

	if (!findBackwards)
	{
		return forwardPoints;
	}

	point = startingPoint;
	std::vector<PointPair> backwardsPoints{};

	for (int i = 0; i < maxPoints - forwardPoints.size(); ++i)
	{
		backwardsPoints.push_back(point);
		if (outsideDomain(point, surfaces))
		{
			break;
		}

		std::optional<PointPair> prevPoint = std::nullopt;
		if (i > 0)
		{
			prevPoint = backwardsPoints[backwardsPoints.size() - 2];
		}

		std::optional<PointPair> newPoint = newtonMethod(surfaces, prevPoint, point, true);
		if (!newPoint.has_value())
		{
			break;
		}
		point = *newPoint;
	}

	std::reverse(backwardsPoints.begin(), backwardsPoints.end());
	backwardsPoints.pop_back();
	backwardsPoints.insert(backwardsPoints.end(), forwardPoints.begin(), forwardPoints.end());

	return backwardsPoints;
}

std::optional<IntersectionCurve::PointPair> IntersectionCurve::newtonMethod(
	const std::array<const Intersectable*, 2>& surfaces, const std::optional<PointPair>& prevPoint,
	const PointPair& startingPoint, bool backwards)
{
	static constexpr float error = 1e-8f;
	static constexpr std::size_t maxIterations = static_cast<std::size_t>(1e4f);

	std::array<glm::vec3, 2> normal{};
	for (std::size_t i = 0; i < 2; ++i)
	{
		normal[i] = glm::cross(surfaces[i]->surfaceDU(startingPoint[i]),
			surfaces[i]->surfaceDV(startingPoint[i]));
	}
	glm::vec3 tangent = glm::normalize(glm::cross(normal[0], normal[1]));
	if (backwards)
	{
		tangent *= -1;
	}

	glm::vec3 startingScenePoint = surfaces[0]->surface(startingPoint[0]);
	if (prevPoint.has_value())
	{
		glm::vec3 prevScenePoint = surfaces[0]->surface((*prevPoint)[0]);
		if (glm::dot(tangent, startingScenePoint - prevScenePoint) < 0)
		{
			tangent *= -1;
		}
	}

	glm::vec4 points = {startingPoint[0], startingPoint[1]};
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

float IntersectionCurve::getDistanceSquared(const glm::vec2& pos1, const glm::vec2& pos2)
{
	glm::vec2 diff = pos2 - pos1;
	return glm::dot(diff, diff);
}

float IntersectionCurve::getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2)
{
	glm::vec3 diff = pos2 - pos1;
	return glm::dot(diff, diff);
}

bool IntersectionCurve::outsideDomain(const PointPair& point,
	const std::array<const Intersectable*, 2>& surfaces)
{
	return !surfaces[0]->uWrapped() && (point[0].x < 0 || point[0].x > 1) ||
		!surfaces[0]->vWrapped() && (point[0].y < 0 || point[0].y > 1) ||
		!surfaces[1]->uWrapped() && (point[1].x < 0 || point[1].x > 1) ||
		!surfaces[1]->vWrapped() && (point[1].y < 0 || point[1].y > 1);
}
