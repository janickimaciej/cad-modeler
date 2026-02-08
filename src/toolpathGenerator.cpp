#include "toolpathGenerator.hpp"

#include "gui/leftPanel.hpp"
#include "models/intersectable.hpp"
#include "models/intersectionCurve.hpp"
#include "scene.hpp"
#include "shaderPrograms.hpp"

#include <format>
#include <fstream>
#include <limits>

static constexpr float nearPlane = 0.1f;
static constexpr float farPlane = 1000.0f;

static inline constexpr float baseHeight = 2.5f;
static inline constexpr float yDefault = 6.6f;

static inline constexpr float roughingPathRadius = 0.8f;
static inline constexpr float roughingPathSegmentingOffset = 0.02f;
static inline constexpr float roughingPathInaccuracyOffset = 0.02f;
static inline constexpr float roughingPathOffset =
	roughingPathRadius + roughingPathSegmentingOffset + roughingPathInaccuracyOffset;
static inline constexpr float roughingIntermediateLevel = 1.25f;

static inline constexpr float flatPathRadius = 0.5f;

static inline constexpr float finishingPathRadius = 0.4f;
static inline constexpr float finishingPathBaseOffset = 0.01f;

ToolpathGenerator::ToolpathGenerator(const Scene& scene) :
	m_scene{scene},
	m_heightmapCamera{m_heightmapSize, 15, nearPlane, farPlane}
{
	m_heightmapCamera.addPitch(glm::radians(-90.0f));
}

void ToolpathGenerator::generatePaths()
{
	generateHeightmap();

	auto roughingPath = generateRoughingPath();
	save(roughingPath, roughingPathRadius, "path1.k16");

	auto flatPath = generateFlatPath();

	auto flatContourPath = generateContourPath(baseHeight);
	float safeHeight = baseHeight + 1.0f;
	float zStart = -7.5f - flatPathRadius * 1.2f;
	glm::vec3 firstPoint = *flatContourPath.begin();
	flatContourPath.insert(flatContourPath.begin(), {firstPoint.x, firstPoint.y, zStart});
	flatContourPath.insert(flatContourPath.begin(), {firstPoint.x, safeHeight, zStart});
	flatContourPath.push_back({firstPoint.x, firstPoint.y, zStart});
	flatContourPath.push_back({firstPoint.x, safeHeight, zStart});

	flatPath.insert(flatPath.end(), flatContourPath.begin(), flatContourPath.end());
	flatPath.insert(flatPath.begin(), {0, yDefault, 0});
	flatPath.push_back({0, yDefault, 0});

	save(flatPath, 0, "path2.f10");

	auto finishingPath = generateFinishingPath();

	auto intersectionsPath = generateIntersectionsPath();

	auto finishingContourPath = generateContourPath(baseHeight + finishingPathRadius);
	firstPoint = *finishingContourPath.begin();
	finishingContourPath.insert(finishingContourPath.begin(),
		{firstPoint.x, safeHeight, firstPoint.z});
	finishingContourPath.push_back({firstPoint.x, safeHeight, firstPoint.z});

	firstPoint = *intersectionsPath.begin();
	finishingPath.push_back({firstPoint.x, safeHeight + finishingPathRadius, firstPoint.z});
	finishingPath.insert(finishingPath.end(), intersectionsPath.begin(), intersectionsPath.end());
	finishingPath.insert(finishingPath.end(), finishingContourPath.begin(),
		finishingContourPath.end());
	finishingPath.insert(finishingPath.begin(), {0, yDefault + finishingPathRadius, 0});
	finishingPath.push_back({0, yDefault + finishingPathRadius, 0});

	save(finishingPath, finishingPathRadius, "path3.k08");
}

std::vector<glm::vec3> ToolpathGenerator::generateRoughingPath()
{
	constexpr float zStart = -7.6f;
	constexpr float xOffset = roughingPathRadius * 1.2f;

	auto generate = [this] (std::vector<glm::vec3>& path, float pathLevel,
		bool backwards, bool left)
		{
			auto offsetHeightmapData = getHeightmapData(m_offsetHeightmap);

			constexpr float stride = roughingPathRadius;
			constexpr int passCount = 20;
			float lowestHeight = baseHeight + pathLevel + roughingPathOffset;

			auto getHeight = [this, lowestHeight, &offsetHeightmapData] (int xIndex, float z)
				{
					return getHeightmapHeight(lowestHeight, *offsetHeightmapData, xIndex, z);
				};

			auto xIndexToX = [] (int xIndex)
				{
					return (xIndex + 0.5f) / m_heightmapSize.x * 15.0f - 7.5f;
				};

			float dx = 15.0f / m_heightmapSize.x;

			for (int i = 0; i < passCount; ++i)
			{
				float xStart = -7.5 - xOffset;
				float xEnd = 7.5 + xOffset;
				if (left)
				{
					std::swap(xStart, xEnd);
				}
				float z = zStart + (backwards ? passCount - 1 - i : i) * stride;
				path.push_back({xStart, lowestHeight, z});

				float xPrev = xStart;
				float yPrev = lowestHeight;

				float heightPrevPix = getHeight(left ? m_heightmapSize.x : -1, z);
				float heightCurrPix = getHeight(left ? m_heightmapSize.x - 1 : 0, z);

				float minCurvatureRadius = std::numeric_limits<float>::max();

				for (int xIndex = 0; xIndex < m_heightmapSize.x; ++xIndex)
				{
					float heightNextPix =
						getHeight(left ? m_heightmapSize.x - 2 - xIndex : xIndex + 1, z);

					float firstDeriv = (heightNextPix - heightPrevPix) / (2.0f * dx);
					float secondDeriv =
						(heightNextPix - 2 * heightCurrPix + heightPrevPix) / (dx * dx);

					constexpr float eps = 1e-12f;
					float curvatureRadius = std::pow(1 + firstDeriv * firstDeriv, 1.5f) /
						(std::abs(secondDeriv) + eps);
					minCurvatureRadius = std::min(minCurvatureRadius, curvatureRadius);

					float x = xIndexToX(left ? m_heightmapSize.x - 1 - xIndex : xIndex);
					float segmentLengthSquared =
						std::pow(x - xPrev, 2.0f) + std::pow(heightCurrPix - yPrev, 2.0f);
					if (segmentLengthSquared >
						4 * (2 * minCurvatureRadius * roughingPathSegmentingOffset -
						std::pow(roughingPathSegmentingOffset, 2)))
					{
						xPrev = xIndexToX(left ? m_heightmapSize.x - xIndex : xIndex - 1);
						yPrev = heightPrevPix;
						path.push_back({xPrev, yPrev, z});
						minCurvatureRadius = std::numeric_limits<float>::max();
					}

					heightPrevPix = heightCurrPix;
					heightCurrPix = heightNextPix;
				}

				path.push_back({xEnd, lowestHeight, z});

				left = !left;
			}
		};

	std::vector<glm::vec3> path{};
	path.push_back({0, yDefault + roughingPathRadius, 0});
	path.push_back({-7.5f - xOffset, yDefault + roughingPathRadius, zStart});

	generateOffsetHeightmap(roughingPathOffset, false, roughingIntermediateLevel);
	generate(path, roughingIntermediateLevel, false, false);
	generateOffsetHeightmap(roughingPathOffset, false);
	generate(path, 0, true, false);

	path.push_back({-7.5f - xOffset, yDefault + roughingPathRadius, zStart});
	path.push_back({0, yDefault + roughingPathRadius, 0});

	return path;
}

std::vector<glm::vec3> ToolpathGenerator::generateFlatPath()
{
	constexpr float zStart = -7.6f;
	constexpr float xOffset = flatPathRadius * 1.2f;
	constexpr float stride = 1.9f * flatPathRadius;
	constexpr int passCount = 16;

	auto generate = [this] (std::vector<glm::vec3>& path, bool backwards)
		{
			auto offsetHeightmapData = getHeightmapData(m_offsetHeightmap);

			constexpr float dz = 15.0f / m_heightmapSize.y;
			constexpr int stridePix = static_cast<int>(stride / dz) + 1;

			auto getHeight = [this, &offsetHeightmapData] (int xIndex, float z)
				{
					return getHeightmapHeight(baseHeight, *offsetHeightmapData, xIndex, z);
				};

			auto xIndexToX = [] (int xIndex)
				{
					return (xIndex + 0.5f) / m_heightmapSize.x * 15.0f - 7.5f;
				};

			auto getLastZero = [backwards, getHeight] (float z)
				{
					int xIndex = backwards ? m_heightmapSize.x - 1 : 0;
					int increment = backwards ? -1 : 1;
					for (int i = 0; i <= m_heightmapSize.x / 2; ++i)
					{
						xIndex += increment;
						constexpr float eps = 1e-9f;
						if (getHeight(xIndex, z) > baseHeight + eps)
						{
							break;
						}
					}
					return xIndex - increment;
				};

			for (int i = 0; i < passCount; i += 2)
			{
				float xStart = -7.5f - xOffset;

				if (backwards)
				{
					xStart = -xStart;
				}

				float z = zStart + (backwards ? passCount - 1 - i : i) * stride;
				float z1 = z;
				float z2 = z + (backwards ? -stride : stride);
				path.push_back({xStart, baseHeight, z1});

				int xIndex1 = getLastZero(z1);
				float x1 = xIndexToX(xIndex1);
				float xLine1 = x1;
				int xIndex2 = getLastZero(z2);
				float x2 = xIndexToX(xIndex2);
				float xLine2 = x2;

				float angle = std::atan((x2 - x1) / (backwards ? -stride : stride));
				constexpr float maxAngle = glm::radians(45.0f);

				if (angle < -maxAngle)
				{
					path.push_back({x1, baseHeight, z1});
					xLine1 = xLine2 + (backwards ? -stride : stride) * std::tan(maxAngle);
				}
				if (angle > maxAngle)
				{
					xLine2 = xLine1 + (backwards ? -stride : stride) * std::tan(maxAngle);
				}

				float maxXOffset = 0;
				for (int j = 0; j < stridePix; ++j)
				{
					float zOffset = j * dz;
					int xIndex = getLastZero(z1 + (backwards ? -zOffset : zOffset));
					float x = xIndexToX(xIndex);
					float xLine = xLine1 + (xLine2 - xLine1) * zOffset / stride;
					maxXOffset = std::max(maxXOffset, (backwards ? x - xLine : xLine - x));
				}
				path.push_back({xLine1 + (backwards ? maxXOffset : -maxXOffset), baseHeight, z1});
				path.push_back({xLine2 + (backwards ? maxXOffset : -maxXOffset), baseHeight, z2});

				if (angle > maxAngle)
				{
					path.push_back({x2, baseHeight, z2});
				}

				path.push_back({xStart, baseHeight, z2});
			}
		};

	float safeHeight = baseHeight + 1.0f;

	std::vector<glm::vec3> path{};
	path.push_back({-7.5f - xOffset, safeHeight, zStart});

	generateOffsetHeightmap(flatPathRadius, true);
	generate(path, false);
	path.push_back({-7.5f - xOffset, baseHeight, zStart + passCount * stride});
	path.push_back({7.5f + xOffset, baseHeight, zStart + passCount * stride});
	generate(path, true);

	path.push_back({7.5f + xOffset, safeHeight, zStart});

	return path;
}

std::vector<glm::vec3> ToolpathGenerator::generateContourPath(float level)
{
	generateEdge(level);

	auto edgeData = getHeightmapData(m_edge);

	auto isEdge = [&edgeData] (int xIndex, int yIndex)
		{
			if (xIndex < 0 || xIndex >= m_heightmapSize.x || yIndex < 0 ||
				yIndex >= m_heightmapSize.y)
			{
				return false;
			}

			return (*edgeData)[yIndex][xIndex] > 0.5f;
		};

	auto getFirstPoint = [&isEdge] ()
		{
			for (int yIndex = 0; yIndex < m_heightmapSize.y; ++yIndex)
			{
				for (int xIndex = 0; xIndex < m_heightmapSize.x; ++xIndex)
				{
					if (isEdge(xIndex, yIndex))
					{
						return glm::ivec2{xIndex, yIndex};
					}
				}
			}
			return glm::ivec2{};
		};

	using Neighbors = std::pair<glm::ivec2, glm::ivec2>;

	auto findNeighbors = [&isEdge] (const glm::ivec2& point)
		{
			glm::ivec2 first{};
			bool findSecond = false;
			for (int i = -1; i <= 1; ++i)
			{
				for (int j = -1; j <= 1; ++j)
				{
					if (i == 0 && j == 0)
					{
						continue;
					}

					if (isEdge(point.x + i, point.y + j))
					{
						glm::ivec2 neighbor{point.x + i, point.y + j};
						if (findSecond)
						{
							return Neighbors{first, neighbor};
						}
						else
						{
							first = neighbor;
							findSecond = true;
						}
					}
				}
			}
			return Neighbors{};
		};

	glm::ivec2 firstPoint = getFirstPoint();
	auto firstPointNeighbors = findNeighbors(firstPoint);
	Neighbors firstPointMoves{firstPointNeighbors.first - firstPoint,
		firstPointNeighbors.second - firstPoint};

	std::vector<glm::ivec2> pointIndices{};
	pointIndices.push_back(firstPoint);
	glm::ivec2 move = firstPointMoves.first.x > 0 ? firstPointMoves.first : firstPointMoves.second;
	glm::ivec2 point = firstPoint + move;

	while (point != firstPoint)
	{
		pointIndices.push_back(point);
		auto neighbors = findNeighbors(point);
		Neighbors moves{neighbors.first - point, neighbors.second - point};
		move = moves.first == -move ? moves.second : moves.first;
		point += move;
	}

	auto indicesToPoint = [level] (const glm::ivec2& point)
		{
			glm::vec2 xz = (glm::vec2{point} + 0.5f) / glm::vec2{m_heightmapSize} * 15.0f - 7.5f;
			return glm::vec3{xz.x, level, xz.y};
		};

	std::vector<glm::vec3> points{};
	for (const auto& pointIndex : pointIndices)
	{
		points.push_back(indicesToPoint(pointIndex));
	}

	auto getProperIndex = [size = points.size()] (int index)
		{
			if (index < 0)
			{
				return index + size;
			}
			if (index >= size)
			{
				return index - size;
			}
			return static_cast<std::size_t>(index);
		};

	auto getSmoothPoint = [&points, &getProperIndex] (int index)
		{
			return 0.15f * points[getProperIndex(index - 3)] +
				0.15f * points[getProperIndex(index - 2)] +
				0.2f * points[getProperIndex(index - 1)] +
				0.0f * points[getProperIndex(index)] +
				0.2f * points[getProperIndex(index + 1)] +
				0.15f * points[getProperIndex(index + 2)] +
				0.15f * points[getProperIndex(index + 3)];
		};

	glm::vec3 point0 = getSmoothPoint(0);

	std::vector<glm::vec3> path{};

	float minCurvatureRadius = std::numeric_limits<float>::max();
	glm::vec3 prevPathPoint = point0;
	path.push_back(prevPathPoint);
	for (int i = 2; i < points.size(); ++i)
	{
		static constexpr int range = 15;
		float curvatureRadius = getCurvatureRadius(points[getProperIndex(i - range)], points[i],
			points[getProperIndex(i + range)]);
		minCurvatureRadius = std::min(minCurvatureRadius, curvatureRadius);

		float segmentLengthSquared = glm::dot(points[i] - prevPathPoint, points[i] - prevPathPoint);
		constexpr float maxDepth = 0.001f;
		if (segmentLengthSquared > 4 * (2 * minCurvatureRadius * maxDepth - std::pow(maxDepth, 2)))
		{
			prevPathPoint = getSmoothPoint(i - 1);
			path.push_back(prevPathPoint);
			minCurvatureRadius = std::numeric_limits<float>::max();
		}
	}
	path.push_back(point0);

	return path;
}

std::vector<glm::vec3> ToolpathGenerator::generateFinishingPath()
{
	auto generate = [this] (std::vector<glm::vec3>& path, const BezierSurface& surface,
		int uResolution, const std::function<glm::vec2(const glm::vec2&)>& remapUV,
		std::optional<int> jump, bool turnOnIntersection = false, int intersectionOffset = {})
		{
			auto offsetHeightmapData = getHeightmapData(m_offsetHeightmap);

			float lowestHeight = baseHeight + finishingPathRadius;
			float safeHeight = baseHeight + finishingPathRadius + 1.0f;

			auto getHeight = [&offsetHeightmapData, lowestHeight] (float x, float z)
				{
					return getHeightmapHeight(lowestHeight, *offsetHeightmapData, x, z);
				};

			auto getPathPoint = [&surface] (const glm::vec2& uv)
				{
					glm::vec3 surfacePoint = surface.surface(uv[0], uv[1]);
					glm::vec3 normalVector = glm::normalize(
						glm::cross(surface.surfaceDU(uv[0], uv[1]),
							surface.surfaceDV(uv[0], uv[1])));
					glm::vec3 pathPoint = surfacePoint + finishingPathRadius * normalVector;
					pathPoint.y += baseHeight;
					pathPoint.z *= -1;
					return pathPoint;
				};

			auto getPoint =
				[&surface, &getHeight, &getPathPoint, remapUV, lowestHeight] (glm::vec2 uv)
				{
					uv = remapUV(uv);
					static constexpr float eps = 5e-3f;

					glm::vec3 pathPoint = getPathPoint(uv);
					float heightmapHeight = getHeight(pathPoint.x, pathPoint.z);
					bool onMilledSurface = heightmapHeight - pathPoint.y < eps;
					pathPoint.y = heightmapHeight;
					pathPoint.y = std::max(pathPoint.y, lowestHeight + finishingPathBaseOffset);
					return std::pair<glm::vec3, bool>{pathPoint, onMilledSurface};
				};

			static constexpr int vResolution = 50000;
			float dU = 1.0f / uResolution;
			static constexpr float dV = 1.0f / vResolution;

			auto findStartNextPath = [&getPoint, lowestHeight] (float u, int& vIndex,
				bool backwards, bool intersection, int offset = 0)
				{
					static constexpr float eps = 2e-6f;
					auto [point, onMilledSurface] =
						getPoint({u, (vIndex + (backwards ? -offset : offset)) * dV});

					while (!intersection &&
							point.y > lowestHeight + finishingPathBaseOffset + eps ||
						intersection && (onMilledSurface ||
							point.y <= lowestHeight + finishingPathBaseOffset + eps))
					{
							backwards ? ++vIndex : --vIndex;
							std::tie(point, onMilledSurface) =
								getPoint({u, (vIndex + (backwards ? offset : -offset)) * dV});
							if (backwards ? vIndex >= vResolution : vIndex <= 0)
							{
								break;
							}
					}

					while (!intersection &&
							point.y <= lowestHeight + finishingPathBaseOffset + eps ||
						intersection && !onMilledSurface &&
							point.y > lowestHeight + finishingPathBaseOffset + eps)
					{
						backwards ? --vIndex : ++vIndex;
						std::tie(point, onMilledSurface) =
							getPoint({u, (vIndex + (backwards ? offset : -offset)) * dV});
						if (backwards ? vIndex <= 0 : vIndex >= vResolution)
						{
							break;
						}
					}
				};

			bool backwards = false;
			int vIndex = 0;
			for (int uIndex = 0; uIndex <= uResolution; ++uIndex)
			{
				float u = uIndex * dU;

				if (uIndex != 0 && (!jump.has_value() || uIndex != *jump))
				{
					int vIndexPathChange = vIndex;
					constexpr int uPathChangeResolution = 10;
					float dUPathChange = dU / uPathChangeResolution;

					glm::vec3 prevPoint = path.back();
					glm::vec3 prevPathPoint = prevPoint;

					float uPathChange = u - dU + dUPathChange;

					findStartNextPath(uPathChange, vIndexPathChange, backwards,
						turnOnIntersection && backwards,
						(turnOnIntersection && backwards) ? intersectionOffset : 0);

					auto [currPoint, _] = getPoint({uPathChange, vIndexPathChange * dV});

					float minVCurvatureRadius = std::numeric_limits<float>::max();
					for (int uPathChangeIndex = 2; uPathChangeIndex < uPathChangeResolution;
						++uPathChangeIndex)
					{
						uPathChange = u - dU + uPathChangeIndex * dUPathChange;

						findStartNextPath(uPathChange, vIndexPathChange, backwards,
							turnOnIntersection && backwards,
							(turnOnIntersection && backwards) ? intersectionOffset : 0);

						auto [nextPoint, __] = getPoint({uPathChange, vIndexPathChange * dV});

						float vCurvatureRadius = getCurvatureRadius(prevPoint, currPoint,
							nextPoint);
						minVCurvatureRadius = std::min(minVCurvatureRadius, vCurvatureRadius);

						float segmentLengthSquared =
							glm::dot(nextPoint - prevPathPoint, nextPoint - prevPathPoint);
						static constexpr float maxDepth = 0.001f;

						if (minVCurvatureRadius < maxDepth ||
							segmentLengthSquared > 4 * (2 * minVCurvatureRadius * maxDepth -
								std::pow(maxDepth, 2)))
						{
							path.push_back(currPoint);
							prevPathPoint = currPoint;
							minVCurvatureRadius = std::numeric_limits<float>::max();
						}

						prevPoint = currPoint;
						currPoint = nextPoint;
					}
				}

				float minVCurvatureRadius = std::numeric_limits<float>::max();

				findStartNextPath(u, vIndex, backwards, turnOnIntersection && backwards,
					(turnOnIntersection && backwards) ? intersectionOffset : 0);

				auto [prevPoint, prevOnMilledSurface] = getPoint({u, vIndex * dV});
				if (uIndex == 0)
				{
					path.push_back({prevPoint.x, safeHeight, prevPoint.z});
				}
				if (jump.has_value() && uIndex == *jump)
				{
					static constexpr float safeChangePathOffset = 0.1f;
					path.push_back(path.back() + glm::vec3{0, safeChangePathOffset, 0});
					path.push_back(prevPoint + glm::vec3{0, safeChangePathOffset, 0});
				}
				path.push_back(prevPoint);
				glm::vec3 prevPathPoint = prevPoint;
				backwards ? --vIndex : ++vIndex;
				auto [currPoint, currOnMilledSurface] = getPoint({u, vIndex * dV});
				glm::vec3 nextPoint{};
				bool nextOnMilledSurface{};

				auto limitRange = [] (int index)
					{
						return std::min(std::max(0, index), vResolution);
					};

				backwards ? --vIndex : ++vIndex;
				for (;; backwards ? --vIndex : ++vIndex)
				{
					std::tie(nextPoint, nextOnMilledSurface) = getPoint({u, vIndex * dV});
					int range = 10;
					auto [negRange, _] = getPoint({u, limitRange(vIndex - 1 - range) * dV});
					auto [posRange, __] = getPoint({u, limitRange(vIndex - 1 + range) * dV});
					float vCurvatureRadius = getCurvatureRadius(negRange, currPoint, posRange);
					minVCurvatureRadius = std::min(minVCurvatureRadius, vCurvatureRadius);

					float segmentLengthSquared =
						glm::dot(nextPoint - prevPathPoint, nextPoint - prevPathPoint);
					static constexpr float maxDepth = 0.001f;
					static constexpr float eps = 1e-6f;
					auto [testPoint, testOnMilledSurface] =
						getPoint({u, (vIndex + (turnOnIntersection && !backwards ?
							intersectionOffset : 0)) * dV});
					if (vIndex == -1 || vIndex == vResolution + 1 ||
						turnOnIntersection && !backwards && !testOnMilledSurface &&
							testPoint.y > lowestHeight + finishingPathBaseOffset + eps ||
						(!turnOnIntersection || backwards) &&
							testPoint.y <= lowestHeight + finishingPathBaseOffset + eps)
					{
						path.push_back(currPoint);
						backwards ? ++vIndex : --vIndex;
						break;
					}
					if (!prevOnMilledSurface && currOnMilledSurface && nextOnMilledSurface ||
						prevOnMilledSurface && currOnMilledSurface && !nextOnMilledSurface ||
						minVCurvatureRadius < maxDepth ||
							segmentLengthSquared > 4 * (2 * minVCurvatureRadius * maxDepth -
								std::pow(maxDepth, 2)))
					{
						path.push_back(currPoint);
						prevPathPoint = currPoint;
						minVCurvatureRadius = std::numeric_limits<float>::max();
					}

					prevPoint = currPoint;
					currPoint = nextPoint;
					prevOnMilledSurface = currOnMilledSurface;
					currOnMilledSurface = nextOnMilledSurface;
				}

				backwards = !backwards;
			}

			glm::vec3 lastPoint = path.back();
			lastPoint.y = safeHeight;
			path.push_back(lastPoint);
		};

	std::vector<glm::vec3> path{};
	generateOffsetHeightmap(finishingPathRadius, false);

	auto surface0Adjust = [] (float u)
		{
			static constexpr float start = 0.126f;
			static constexpr float end = 0.885f;
			return start + (end - start) * u;
		};

	auto surface1Adjust = [] (float u)
		{
			static constexpr float start = 0.12f;
			static constexpr float end = 0.912f;
			return start + (end - start) * u;
		};
	auto surface2Adjust = [] (float u)
		{
			static constexpr float start = 0.114f;
			static constexpr float end = 0.841f;
			return start + (end - start) * u;
		};
	auto surface3Adjust = [] (float u)
		{
			static constexpr float start = 0.248f;
			static constexpr float end = 0.88f;
			return start + (end - start) * u;
		};

	generate(path, *m_scene.m_c0BezierSurfaces[0], 38, [&surface0Adjust] (const glm::vec2& uv)
		{
			return glm::vec2{surface0Adjust(uv[0]) / 2.0f, 0.89f * (1.0f - uv[1]) + 0.11f};
		}, 19);
	generate(path, *m_scene.m_c0BezierSurfaces[1], 37, [&surface1Adjust] (const glm::vec2& uv)
		{
			return glm::vec2{uv[1], 0.5f + surface1Adjust(1.0f - uv[0]) / 2.0f};
		}, std::nullopt, true, 100);
	generate(path, *m_scene.m_c0BezierSurfaces[1], 37, [&surface1Adjust] (const glm::vec2& uv)
		{
			return glm::vec2{1.0f - uv[1], 0.5f + surface1Adjust(uv[0]) / 2.0f};
		}, std::nullopt, true, 100);
	generate(path, *m_scene.m_c0BezierSurfaces[2], 23, [&surface2Adjust] (const glm::vec2& uv)
		{
			return glm::vec2{uv[1], surface2Adjust(1.0f - uv[0]) / 2.0f};
		}, std::nullopt, true, 300);
	generate(path, *m_scene.m_c0BezierSurfaces[2], 23, [&surface2Adjust] (const glm::vec2& uv)
		{
			return glm::vec2{1.0f - uv[1], surface2Adjust(uv[0]) / 2.0f};
		}, std::nullopt, true, 300);
	generate(path, *m_scene.m_c0BezierSurfaces[3], 20, [&surface3Adjust] (const glm::vec2& uv)
		{
			return glm::vec2{uv[1], 0.5f + surface3Adjust(uv[0]) / 2.0f};
		}, std::nullopt);

	return path;
}

std::vector<glm::vec3> ToolpathGenerator::generateIntersectionsPath()
{
	auto offsetHeightmapData = getHeightmapData(m_offsetHeightmap);

	float lowestHeight = baseHeight + finishingPathRadius;

	auto getHeight = [&offsetHeightmapData, lowestHeight] (float x, float z)
		{
			return getHeightmapHeight(lowestHeight, *offsetHeightmapData, x, z);
		};

	auto generate = [this, &getHeight, lowestHeight] (std::vector<glm::vec3>& path,
		Intersectable& surface0, Intersectable& surface1, const glm::vec3& cursorPos,
		bool invert)
		{
			auto getNormalVec = [] (const Intersectable& surface, const glm::vec2& uv)
				{
					return glm::normalize(glm::cross(surface.surfaceDU(uv[0], uv[1]),
						surface.surfaceDV(uv[0], uv[1])));
				};

			std::unique_ptr<IntersectionCurve> intersectionCurve =
				IntersectionCurve::create({&surface0, &surface1}, 0.001f, cursorPos);
			auto intersectionPoints0 = intersectionCurve->getIntersectionPoints(0);
			auto intersectionPoints1 = intersectionCurve->getIntersectionPoints(1);
			auto intersectionPoints = intersectionCurve->getIntersectionPoints();

			std::vector<glm::vec3> offsetPoints{};
			for (int i = 0; i < intersectionPoints.size(); ++i)
			{
				glm::vec3 normalVec0 = getNormalVec(surface0, intersectionPoints0[i]);
				glm::vec3 normalVec1 = getNormalVec(surface1, intersectionPoints1[i]);

				glm::vec3 middleVec = glm::normalize(normalVec0 + normalVec1);
				float cos = glm::dot(normalVec0, middleVec);
				float offset = finishingPathRadius / cos;

				glm::vec3 surfacePoint = intersectionPoints[i];
				offsetPoints.push_back(surfacePoint + offset * middleVec);
			}

			bool prevPositive = invert ?
				offsetPoints[0].y > finishingPathRadius + finishingPathBaseOffset :
				offsetPoints.back().y > finishingPathRadius + finishingPathBaseOffset;
			int start{};
			int count = static_cast<int>(offsetPoints.size());
			for (int i = 0; i < count; ++i)
			{
				int index = invert ? count - 1 - i : i;
				if (offsetPoints[index].y > finishingPathRadius + finishingPathBaseOffset)
				{
					if (!prevPositive)
					{
						start = index;
						break;
					}
					prevPositive = true;
				}
				else
				{
					prevPositive = false;
				}
			}

			int preStart = invert ? start + 1 : start - 1;
			if (preStart < 0)
			{
				preStart += count;
			}
			if (preStart >= count)
			{
				preStart -= count;
			}

			glm::vec3 prevNormalVec = getNormalVec(surface1, intersectionPoints1[preStart]);
			glm::vec3 prevPoint = offsetPoints[preStart];
			std::vector<glm::vec3> pathPoints{};
			for (int i = 0; i < count; ++i)
			{
				int index = invert ? start - i : start + i;
				if (index < 0)
				{
					index += count;
				}
				if (index >= count)
				{
					index -= count;
				}

				glm::vec3 normalVec = getNormalVec(surface1, intersectionPoints1[index]);

				if (offsetPoints[index].y <= finishingPathRadius + finishingPathBaseOffset ||
					glm::length(offsetPoints[index] - prevPoint) > 0.05f)
				{
					break;
				}
				pathPoints.push_back(offsetPoints[index]);
				prevNormalVec = normalVec;
				prevPoint = offsetPoints[index];
			}

			for (auto& point : pathPoints)
			{
				point.z *= -1;
				point.y += baseHeight;
			}

			glm::vec3 firstPoint = pathPoints[0];
			float safeHeight = baseHeight + finishingPathRadius + 0.3f;
			path.push_back({firstPoint.x, safeHeight, firstPoint.z});

			float minCurvatureRadius = std::numeric_limits<float>::max();
			glm::vec3 prevPathPoint = firstPoint;
			path.push_back(prevPathPoint);
			for (int i = 2; i < pathPoints.size(); ++i)
			{
				float curvatureRadius = getCurvatureRadius(pathPoints[i - 1], pathPoints[i],
					pathPoints[i + 1]);
				minCurvatureRadius = std::min(minCurvatureRadius, curvatureRadius);

				float segmentLengthSquared = glm::dot(pathPoints[i] - prevPathPoint,
					pathPoints[i] - prevPathPoint);
				constexpr float maxDepth = 0.001f;
				if (segmentLengthSquared >
					4 * (2 * minCurvatureRadius * maxDepth - std::pow(maxDepth, 2)))
				{
					prevPathPoint = pathPoints[i - 1];
					path.push_back(prevPathPoint);
					minCurvatureRadius = std::numeric_limits<float>::max();
				}
			}
			glm::vec3 lastPoint = pathPoints.back();
			path.push_back(lastPoint);

			path.push_back({lastPoint.x, safeHeight, lastPoint.z});
		};

	std::vector<glm::vec3> path{};
	generate(path, *m_scene.m_c0BezierSurfaces[0], *m_scene.m_c0BezierSurfaces[1],
		{-0.8f, 0.5f, 3.0f}, false);
	generate(path, *m_scene.m_c0BezierSurfaces[0], *m_scene.m_c0BezierSurfaces[2],
		{-0.9f, 0.4f, -0.3f}, false);
	generate(path, *m_scene.m_c0BezierSurfaces[0], *m_scene.m_c0BezierSurfaces[3],
		{0.1f, 0.2f, -4.3f}, false);
	generate(path, *m_scene.m_c0BezierSurfaces[0], *m_scene.m_c0BezierSurfaces[3],
		{0.1f, 0.2f, -4.3f}, true);
	generate(path, *m_scene.m_c0BezierSurfaces[0], *m_scene.m_c0BezierSurfaces[2],
		{-0.1f, 0.4f, -0.3f}, false);
	generate(path, *m_scene.m_c0BezierSurfaces[0], *m_scene.m_c0BezierSurfaces[1],
		{0.5f, 0.5f, 3.0f}, false);

	return path;
}

void ToolpathGenerator::generateHeightmap()
{
	m_heightmap.bind();
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_heightmapCamera.use();
	ShaderPrograms::bezierSurfaceHeight->use();
	for (const auto& surface : m_scene.m_bezierPatches)
	{
		surface->m_mesh->render();
	}
	m_heightmap.unbind();
}

void ToolpathGenerator::generateOffsetHeightmap(float radius, bool flatCutter, float pathLevel)
{
	m_offsetHeightmap.bind();
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_offsetHeightmap.unbind();

	static constexpr glm::ivec2 viewportSize{75, 75};
	for (int i = 0; i * viewportSize.x < m_heightmapSize.x; ++i)
	{
		for (int j = 0; j * viewportSize.y < m_heightmapSize.y; ++j)
		{
			glm::ivec2 viewportOffset{i * viewportSize.x, j * viewportSize.y};
			m_offsetHeightmap.bind(viewportOffset, viewportSize);
			ShaderPrograms::heightmap->use();
			ShaderPrograms::heightmap->setUniform("heightmapSize", m_heightmapSize);
			ShaderPrograms::heightmap->setUniform("radius", radius);
			ShaderPrograms::heightmap->setUniform("flatCutter", flatCutter);
			ShaderPrograms::heightmap->setUniform("base", baseHeight);
			ShaderPrograms::heightmap->setUniform("pathLevel", pathLevel);
			ShaderPrograms::heightmap->setUniform("viewportSize", viewportSize);
			ShaderPrograms::heightmap->setUniform("viewportOffset", viewportOffset);
			m_heightmap.bindTexture();
			m_quad.render();
			m_offsetHeightmap.unbind();
		}
	}
}

void ToolpathGenerator::generateEdge(float level)
{
	m_edge.bind();
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderPrograms::edge->use();
	ShaderPrograms::edge->setUniform("level", level);
	ShaderPrograms::edge->setUniform("resolution", m_heightmapSize);
	m_offsetHeightmap.bindTexture();
	m_quad.render();
	m_edge.unbind();
}

std::unique_ptr<ToolpathGenerator::HeightmapData> ToolpathGenerator::getHeightmapData(
	Framebuffer<float>& heightmap)
{
	auto textureData = std::make_unique<TextureData>();
	heightmap.bind();
	heightmap.getTextureData((*textureData)[0][0].data());
	heightmap.unbind();
	auto heightmapData = std::make_unique<HeightmapData>();
	for (int i = 0; i < m_heightmapSize.y; ++i)
	{
		for (int j = 0; j < m_heightmapSize.x; ++j)
		{
			(*heightmapData)[i][j] = (*textureData)[i][j][0];
		}
	}
	return heightmapData;
}

float ToolpathGenerator::getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
	int xIndex, float z)
{
	if (xIndex < 0 || xIndex >= m_heightmapSize.x)
	{
		return defaultHeight;
	}
	float yPix = (z + 7.5f) / 15.0f * m_heightmapSize.y - 0.5f;
	if (yPix < 0 || yPix >= m_heightmapSize.y - 1)
	{
		return defaultHeight;
	}
	int yPixFloor = static_cast<int>(glm::floor(yPix));
	int yPixCeil = yPixFloor + 1;
	float heightYFloor = heightmapData[yPixFloor][xIndex];
	float heightYCeil = heightmapData[yPixCeil][xIndex];
	return (yPix - yPixFloor) * heightYCeil + (yPixCeil - yPix) * heightYFloor;
}

float ToolpathGenerator::getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
	float x, float z)
{
	float xPix = (x + 7.5f) / 15.0f * m_heightmapSize.x - 0.5f;
	float yPix = (z + 7.5f) / 15.0f * m_heightmapSize.y - 0.5f;

	if (xPix < 0 || xPix >= m_heightmapSize.x - 1 || yPix < 0 || yPix >= m_heightmapSize.y - 1)
	{
		return defaultHeight;
	}

	int xPixFloor = static_cast<int>(glm::floor(xPix));
	int xPixCeil = xPixFloor + 1;
	int yPixFloor = static_cast<int>(glm::floor(yPix));
	int yPixCeil = yPixFloor + 1;
	float heightYFloor = (xPix - xPixFloor) * heightmapData[yPixFloor][xPixCeil] +
		(xPixCeil - xPix) * heightmapData[yPixFloor][xPixFloor];
	float heightYCeil = (xPix - xPixFloor) * heightmapData[yPixCeil][xPixCeil] +
		(xPixCeil - xPix) * heightmapData[yPixCeil][xPixFloor];
	return (yPix - yPixFloor) * heightYCeil + (yPixCeil - yPix) * heightYFloor;
}

float ToolpathGenerator::getCurvatureRadius(const glm::vec3& p1, const glm::vec3& p2,
	const glm::vec3& p3)
{
	glm::vec3 v1 = p1 - p3;
	glm::vec3 v2 = p2 - p3;

	float sin = glm::length(glm::cross(v1, v2)) / (glm::length(v1) * glm::length(v2));
	constexpr float eps = 1e-6f;
	if (sin < eps)
	{
		return std::numeric_limits<float>::max();
	}

	return glm::length(p2 - p1) / (2 * sin);
}

void ToolpathGenerator::save(const std::vector<glm::vec3>& path, float yOffset,
	const std::string& filename)
{
	std::ofstream file{filename};
	for (int i = 0; i < path.size(); ++i)
	{
		file << std::format("N{}G01X{:.3f}Y{:.3f}Z{:.3f}\n", i + 3, path[i].x * 10, path[i].z * 10,
			(path[i].y - yOffset) * 10);
	}
	file.close();
}
