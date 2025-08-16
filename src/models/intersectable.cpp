#include "models/intersectable.hpp"

Intersectable::Intersectable(const glm::vec3& pos, const std::string& name,
	const ChangeCallback& changeCallback) :
	Model{pos, name},
	m_changeCallback{changeCallback}
{ }

void Intersectable::select()
{
	m_selectedIntersectionCurve = std::nullopt;
	Model::select();
}

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
}

int Intersectable::intersectionCurveCount() const
{
	return static_cast<int>(m_intersectionCurves.size());
}

std::string Intersectable::intersectionCurveName(int index) const
{
	return m_intersectionCurves[index]->getName();
}

std::optional<int> Intersectable::selectedIntersectionCurveIndex() const
{
	return m_selectedIntersectionCurve;
}

void Intersectable::selectIntersectionCurve(std::optional<int> index)
{
	m_selectedIntersectionCurve = index;
}

Intersectable::Trim Intersectable::getIntersectionCurveTrim(int index) const
{
	return m_intersectionCurveTrims[index];
}

void Intersectable::setIntersectionCurveTrim(int index, Trim trim)
{
	m_intersectionCurveTrims[index] = trim;
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
	m_selectedIntersectionCurve = std::nullopt;
}

int Intersectable::getCurveIndex(const IntersectionCurve* curve) const
{
	auto iterator = std::find(m_intersectionCurves.begin(), m_intersectionCurves.end(), curve);
	return static_cast<int>(iterator - m_intersectionCurves.begin());
}
