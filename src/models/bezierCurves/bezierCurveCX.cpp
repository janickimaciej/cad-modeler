#include "models/bezierCurves/bezierCurveCX.hpp"

#include <cstddef>

BezierCurveCX::BezierCurveCX(const std::string& name, const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
	const SelfDestructCallback& selfDestructCallback) :
	BezierCurve{name, curveShaderProgram, polylineShaderProgram, points, selfDestructCallback}
{ }

std::vector<glm::vec3> BezierCurveCX::pointsToCurveVertices(const std::vector<Point*> points)
{
	if (points.size() == 0)
	{
		return {};
	}

	std::vector<glm::vec3> vertices{};
	std::size_t patchCount = (points.size() - 1) / 3;
	for (std::size_t i = 0; i < patchCount; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			vertices.push_back(points[3 * i + j]->getPos());
		}
	}
	return vertices;
}

void BezierCurveCX::renderCurve() const
{
	if (m_points.size() >= 4)
	{
		useCurveShaderProgram();
		m_curveMesh->render();
	}
}
