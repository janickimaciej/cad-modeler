#pragma once

#include "models/bezierCurves/bezierCurveCX.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <vector>

class BezierCurveC0 : public BezierCurveCX
{
public:
	BezierCurveC0(const ShaderProgram& bezierCurveShaderProgram,
		const ShaderProgram& bezierCurvePolylineShaderProgram, const std::vector<Point*>& points,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~BezierCurveC0() = default;
	void addPoints(const std::vector<Point*>& points);

private:
	static int m_count;

	virtual void createCurveMesh() override;
	virtual void updateCurveMesh() override;
};
