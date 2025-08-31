#pragma once

#include "models/bezierCurves/cxBezierCurve.hpp"
#include "models/point.hpp"

#include <vector>

class C0BezierCurve : public CXBezierCurve
{
	friend class C0BezierCurveSerializer;

public:
	C0BezierCurve(const std::vector<Point*>& points,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~C0BezierCurve() = default;
	void addPoints(const std::vector<Point*>& points);

private:
	static int m_count;

	virtual void createCurveMesh() override;
	virtual void updateCurveMesh() override;
};
