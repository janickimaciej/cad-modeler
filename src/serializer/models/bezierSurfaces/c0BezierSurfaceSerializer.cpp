#include "serializer/models/bezierSurfaces/c0BezierSurfaceSerializer.hpp"

#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"

nlohmann::ordered_json C0BezierSurfaceSerializer::serialize(const C0BezierSurface& surface,
	const std::vector<Point*>& points, int& id)
{
	nlohmann::ordered_json json{};

	json["objectType"] = "bezierSurfaceC0";
	json["name"] = surface.getName();
	json["id"] = id++;

	json["parameterWrapped"]["u"] = surface.m_wrapping == BezierSurfaceWrapping::u;
	json["parameterWrapped"]["v"] = surface.m_wrapping == BezierSurfaceWrapping::v;

	json["size"]["x"] = surface.m_patchesU;
	json["size"]["y"] = surface.m_patchesV;

	for (int patchV = 0; patchV < surface.m_patchesV; ++patchV)
	{
		for (int patchU = 0; patchU < surface.m_patchesU; ++patchU)
		{
			nlohmann::ordered_json patchJson{};
			patchJson["objectType"] = "bezierPatchC0";
			patchJson["name"] = "Patch C0";
			patchJson["id"] = id++;
			patchJson["samples"]["x"] = 4;
			patchJson["samples"]["y"] = 4;
			for (int dV = 0; dV < 4; ++dV)
			{
				for (int dU = 0; dU < 4; ++dU)
				{
					nlohmann::ordered_json pointJson{};
					int u = (3 * patchU + dU) % static_cast<int>(surface.m_pointsU);
					int v = (3 * patchV + dV) % static_cast<int>(surface.m_pointsV);
					auto pointIterator = std::find_if
					(
						points.begin(), points.end(),
						[surfacePoint = surface.m_points[v][u]] (const Point* point)
						{
							return point == surfacePoint;
						}
					);
					pointJson["id"] = pointIterator - points.begin();
					patchJson["controlPoints"].push_back(pointJson);
				}
			}
			json["patches"].push_back(patchJson);
		}
	}

	return json;
}

void C0BezierSurfaceSerializer::deserialize(const nlohmann::ordered_json& json, Scene& scene,
	const std::unordered_map<int, int>& pointMap)
{
	BezierSurfaceWrapping wrapping = BezierSurfaceWrapping::none;
	if (json["parameterWrapped"]["u"])
	{
		wrapping = BezierSurfaceWrapping::u;
	}
	else if (json["parameterWrapped"]["v"])
	{
		wrapping = BezierSurfaceWrapping::v;
	}

	int patchesU = json["size"]["x"];
	int patchesV = json["size"]["y"];

	std::vector<std::unique_ptr<Point>> points{};
	std::vector<std::unique_ptr<BezierPatch>> patches{};
	std::unique_ptr<C0BezierSurface> surface = std::make_unique<C0BezierSurface>(
		[&scene] (const std::vector<IntersectionCurve*>& intersectionCurves)
		{
			scene.deleteIntersectionCurves(intersectionCurves);
		},
		scene.m_shaderPrograms.bezierSurface, scene.m_shaderPrograms.mesh,
		scene.m_shaderPrograms.point, patchesU, patchesV, glm::vec3{}, 1.0f, 1.0f, wrapping,
		points, patches);
	scene.addBezierPatches(std::move(patches));

	surface->m_pointMoveNotifications.clear();
	surface->m_pointRereferenceNotifications.clear();
	surface->m_pointDeletabilityLocks.clear();

	int patchU = 0;
	int patchV = 0;
	for (const nlohmann::ordered_json& patchJson : json["patches"])
	{
		int dU = 0;
		int dV = 0;
		for (const nlohmann::ordered_json& pointJson : patchJson["controlPoints"])
		{
			int u = (3 * patchU + dU) % static_cast<int>(surface->m_pointsU);
			int v = (3 * patchV + dV) % static_cast<int>(surface->m_pointsV);
			Point* point = scene.m_points[pointMap.at(pointJson["id"])].get();
			surface->m_points[v][u] = point;
			point->setDeletable(false);

			++dU;
			if (dU == 4)
			{
				dU = 0;
				++dV;
			}
		}

		++patchU;
		if (patchU == patchesU)
		{
			patchU = 0;
			++patchV;
		}
	}
	surface->updateGeometry();
	surface->registerForNotifications();

	if (json.contains("name"))
	{
		surface->setName(json["name"]);
	}

	scene.m_models.push_back(surface.get());
	scene.m_c0BezierSurfaces.push_back(std::move(surface));
}
