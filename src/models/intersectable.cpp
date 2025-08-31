#include "models/intersectable.hpp"

Intersectable::Intersectable(const glm::vec3& pos, const std::string& name,
	const ChangeCallback& changeCallback) :
	Model{pos, name},
	m_changeCallback{changeCallback}
{ }

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

void Intersectable::addIntersectionCurve(IntersectionCurve* curve, int surfaceIndex)
{
	m_intersectionCurves.push_back(curve);
	registerForNotification(curve);
	m_intersectionCurveTrims.push_back(Trim::none);
	m_intersectionCurveSmallTextures.push_back(
		createIntersectionCurveTexture<smallTextureSize>(curve, surfaceIndex));
	m_intersectionCurveBigTextures.push_back(
		createIntersectionCurveTexture<bigTextureSize>(curve, surfaceIndex));
}

int Intersectable::intersectionCurveCount() const
{
	return static_cast<int>(m_intersectionCurves.size());
}

std::string Intersectable::intersectionCurveName(int index) const
{
	return m_intersectionCurves[index]->getName();
}

Intersectable::Trim Intersectable::getIntersectionCurveTrim(int index) const
{
	return m_intersectionCurveTrims[index];
}

void Intersectable::setIntersectionCurveTrim(int index, Trim trim)
{
	for (Trim& curveTrim : m_intersectionCurveTrims)
	{
		curveTrim = Trim::none;
	}
	m_trimmingCurve = std::nullopt;

	if (trim != Trim::none)
	{
		m_intersectionCurveTrims[index] = trim;
		m_trimmingCurve = index;
	}
}

unsigned int Intersectable::getIntersectionCurveTextureId(int index) const
{
	return m_intersectionCurveSmallTextures[index].getId();
}

void Intersectable::useTrim(const ShaderProgram& surfaceShaderProgram) const
{
	surfaceShaderProgram.use();
	surfaceShaderProgram.setUniform("useTrim", m_trimmingCurve.has_value());
	if (m_trimmingCurve.has_value())
	{
		surfaceShaderProgram.setUniform("trimSide",
			static_cast<int>(m_intersectionCurveTrims[*m_trimmingCurve]));
		m_intersectionCurveBigTextures[*m_trimmingCurve].use();
	}
}

void Intersectable::notifyChange()
{
	m_changeCallback(m_intersectionCurves);
}

void Intersectable::registerForNotification(IntersectionCurve* curve)
{
	m_intersectionCurveDestroyNotifications.push_back(curve->registerForDestroyNotification
		(
			[this] (IntersectionCurve* curve)
			{
				intersectionCurveDestroyNotification(curve);
			}
		));
}

void Intersectable::intersectionCurveDestroyNotification(const IntersectionCurve* curve)
{
	int curveIndex = getCurveIndex(curve);
	m_intersectionCurves.erase(m_intersectionCurves.begin() + curveIndex);
	m_intersectionCurveDestroyNotifications.erase(
		m_intersectionCurveDestroyNotifications.begin() + curveIndex);
	m_intersectionCurveTrims.erase(m_intersectionCurveTrims.begin() + curveIndex);
	m_intersectionCurveSmallTextures.erase(m_intersectionCurveSmallTextures.begin() + curveIndex);
	m_intersectionCurveBigTextures.erase(m_intersectionCurveBigTextures.begin() + curveIndex);
	if (m_trimmingCurve.has_value() && *m_trimmingCurve == curveIndex)
	{
		m_trimmingCurve = std::nullopt;
	}
}

std::unique_ptr<FlatMesh> Intersectable::createIntersectionMesh(
	const std::vector<glm::vec2>& intersectionPoints) const
{
	std::vector<glm::vec2> vertices{};
	std::vector<unsigned int> indices{};

	int index = 0;
	glm::vec2 segmentStart{};
	glm::vec2 segmentEnd = intersectionPoints[0];
	vertices.push_back(params2Tex(segmentEnd));
	for (std::size_t segment = 0; segment < intersectionPoints.size() - 1; ++segment)
	{
		segmentStart = segmentEnd;
		segmentEnd = intersectionPoints[segment + 1];

		bool cross = false;
		glm::vec2 crossDirection{0, 0};
		float xDiff = segmentEnd.x - segmentStart.x;
		float yDiff = segmentEnd.y - segmentStart.y;
		if (xDiff > 0.5f)
		{
			crossDirection += glm::vec2{-1, 0};
			cross = true;
		}
		if (xDiff < -0.5f)
		{
			crossDirection += glm::vec2{1, 0};
			cross = true;
		}
		if (yDiff > 0.5f)
		{
			crossDirection += glm::vec2{0, -1};
			cross = true;
		}
		if (yDiff < -0.5f)
		{
			crossDirection += glm::vec2{0, 1};
			cross = true;
		}

		if (cross)
		{
			vertices.push_back(params2Tex(segmentEnd + crossDirection));
			indices.push_back(index);
			indices.push_back(++index);

			vertices.push_back(params2Tex(segmentStart - crossDirection));
			vertices.push_back(params2Tex(segmentEnd));
			indices.push_back(++index);
			indices.push_back(++index);
		}
		else
		{
			vertices.push_back(params2Tex(segmentEnd));
			indices.push_back(index);
			indices.push_back(++index);
		}
	}

	return std::make_unique<FlatMesh>(vertices, indices);
}

int Intersectable::getCurveIndex(const IntersectionCurve* curve) const
{
	auto iterator = std::find(m_intersectionCurves.begin(), m_intersectionCurves.end(), curve);
	return static_cast<int>(iterator - m_intersectionCurves.begin());
}

glm::vec2 Intersectable::params2Tex(const glm::vec2& parameters)
{
	static constexpr float adjustment = 0.9999f;
	return
	{
		adjustment * (2 * parameters.x - 1),
		adjustment * (2 * parameters.y - 1)
	};
}
