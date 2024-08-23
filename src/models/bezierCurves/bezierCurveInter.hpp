#pragma once

#include "meshes/bezierCurveInterMesh.hpp"
#include "models/bezierCurves/bezierCurve.hpp"
#include "models/bezierCurves/bezierCurveInterSegmentData.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <vector>

class BezierCurveInter : public BezierCurve
{
public:
	BezierCurveInter(const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~BezierCurveInter() = default;

	void addPoints(const std::vector<Point*>& points);

private:
	static int m_count;

	std::unique_ptr<BezierCurveInterMesh> m_curveMesh{};

	virtual void createCurveMesh() override;

	virtual void updateCurveMesh() override;

	virtual void renderCurve() const override;

	std::vector<BezierCurveInterSegmentData> createCurveSegments() const;
};
