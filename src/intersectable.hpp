#pragma once

#include "models/intersectionCurve.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Intersectable
{
public:
	using ChangeCallback = std::function<void(const std::vector<IntersectionCurve*>&)>;

	Intersectable(const ChangeCallback& changeCallback);

	virtual glm::vec3 surface(float u, float v) const = 0;
	virtual glm::vec3 surfaceDU(float u, float v) const = 0;
	virtual glm::vec3 surfaceDV(float u, float v) const = 0;

	glm::vec3 surface(const glm::vec2& pos) const;
	glm::vec3 surfaceDU(const glm::vec2& pos) const;
	glm::vec3 surfaceDV(const glm::vec2& pos) const;

	virtual bool uWrapped() const = 0;
	virtual bool vWrapped() const = 0;

	void addIntersectionCurve(IntersectionCurve* curve);
	int intersectionCurveCount() const;
	std::string intersectionCurveName(int index) const;

protected:
	void notifyChange();

private:
	std::vector<IntersectionCurve*> m_intersectionCurves{};
	std::vector<std::shared_ptr<IntersectionCurve::DestroyCallback>>
		m_intersectionCurveDestroyNotifications{};

	ChangeCallback m_changeCallback;

	void registerForNotification(IntersectionCurve* curve);
	void intersectionCurveDestroyNotification(const IntersectionCurve* curve);
	int getCurveIndex(const IntersectionCurve* curve) const;
};
