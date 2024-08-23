#include "models/bezierCurves/bezierCurveCX.hpp"

BezierCurveCX::BezierCurveCX(const std::string& name, const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
	const SelfDestructCallback& selfDestructCallback) :
	BezierCurve{name, curveShaderProgram, polylineShaderProgram, points, selfDestructCallback}
{ }

std::vector<unsigned int> BezierCurveCX::createCurveIndices(const std::vector<Point*>& points)
{
	std::vector<unsigned int> indices{};
	int patchCount = (static_cast<int>(points.size()) - 1) / 3;
	for (int i = 0; i < patchCount; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			indices.push_back(3 * i + j);
		}
	}
	return indices;
}

void BezierCurveCX::renderCurve() const
{
	if (m_points.size() >= 4)
	{
		useCurveShaderProgram();
		m_curveMesh->render();
	}
}
