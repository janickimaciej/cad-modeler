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

void Intersectable::addIntersectionCurve(IntersectionCurve* curve)
{
	m_intersectionCurves.push_back(curve);
	registerForNotification(curve);
	m_intersectionCurveTrims.push_back(Trim::none);
	m_intersectionCurveTextures.push_back(createIntersectionCurveTexture(curve));
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
	m_intersectionCurveTrims[index] = trim;
}

unsigned int Intersectable::getIntersectionCurveTextureId(int index) const
{
	return m_intersectionCurveTextures[index].getId();
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
}

int Intersectable::getCurveIndex(const IntersectionCurve* curve) const
{
	auto iterator = std::find(m_intersectionCurves.begin(), m_intersectionCurves.end(), curve);
	return static_cast<int>(iterator - m_intersectionCurves.begin());
}

Texture Intersectable::createIntersectionCurveTexture(const IntersectionCurve* curve)
{
	static constexpr int textureSize = 256;
	using TextureData =
		std::array<std::array<std::array<unsigned char, 3>, textureSize>, textureSize>;

	std::vector<glm::vec3> intersectionPoints = curve->getIntersectionPoints();
	std::unique_ptr<TextureData> textureData = std::make_unique<TextureData>();

	for (int i = 0; i < textureSize; ++i)
	{
		for (int j = 0; j < textureSize; ++j)
		{
			(*textureData)[i][j][0] = 0;
			(*textureData)[i][j][1] = 255;
			(*textureData)[i][j][2] = 0;
		}
	}

	Texture texture{textureSize, textureSize, (*textureData)[0][0].data()};
	return texture;
}
