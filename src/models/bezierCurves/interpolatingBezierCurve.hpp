#pragma once

#include "meshes/interpolatingBezierCurveMesh.hpp"
#include "models/bezierCurves/bezierCurve.hpp"
#include "models/bezierCurves/interpolatingBezierCurveSegmentData.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <vector>

class InterpolatingBezierCurve : public BezierCurve
{
	friend class InterpolatingBezierCurveSerializer;

public:
	InterpolatingBezierCurve(const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~InterpolatingBezierCurve() = default;

	void addPoints(const std::vector<Point*>& points);

private:
	static int m_count;

	std::unique_ptr<InterpolatingBezierCurveMesh> m_curveMesh{};

	virtual void createCurveMesh() override;

	virtual void updateCurveMesh() override;

	virtual void renderCurve() const override;

	std::vector<InterpolatingBezierCurveSegmentData> createCurveSegments() const;
};
