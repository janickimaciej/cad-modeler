#include "intersectable.hpp"

Intersectable::Intersectable(const ChangeCallback& changeCallback) :
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
}

int Intersectable::intersectionCurveCount() const
{
	return static_cast<int>(m_intersectionCurves.size());
}

std::string Intersectable::intersectionCurveName(int index) const
{
	return m_intersectionCurves[index]->getName();
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
}

int Intersectable::getCurveIndex(const IntersectionCurve* curve) const
{
	auto iterator = std::find(m_intersectionCurves.begin(), m_intersectionCurves.end(), curve);
	return static_cast<int>(iterator - m_intersectionCurves.begin());
}
