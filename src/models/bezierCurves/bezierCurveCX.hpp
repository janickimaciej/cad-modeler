#pragma once

#include "meshes/bezierCurveMesh.hpp"
#include "models/bezierCurves/bezierCurve.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

class BezierCurveCX : public BezierCurve
{
public:
	BezierCurveCX(const std::string& name, const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
		const SelfDestructCallback& selfDestructCallback);

protected:
	std::unique_ptr<BezierCurveMesh> m_curveMesh{};

	static std::vector<glm::vec3> pointsToCurveVertices(const std::vector<Point*> points);

private:
	virtual void renderCurve() const override;
};
