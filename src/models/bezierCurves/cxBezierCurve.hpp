#pragma once

#include "meshes/indicesMesh.hpp"
#include "models/bezierCurves/bezierCurve.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <string>
#include <vector>

class CXBezierCurve : public BezierCurve
{
public:
	CXBezierCurve(const std::string& name, const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~CXBezierCurve() = default;

protected:
	std::unique_ptr<IndicesMesh> m_curveMesh{};

	static std::vector<unsigned int> createCurveIndices(const std::vector<Point*>& points);

private:
	virtual void renderCurve() const override;
};
