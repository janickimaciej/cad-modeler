#include "models/bezierCurves/bezierCurve.hpp"

BezierCurve::BezierCurve(const std::string& name, const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram) :
	Model{{}, name},
	m_curveShaderProgram{curveShaderProgram},
	m_polylineShaderProgram{polylineShaderProgram}
{ }

void BezierCurve::updateGUI()
{
	m_gui.update();
}

void BezierCurve::setPos(const glm::vec3&)
{ }

void BezierCurve::setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&)
{ }

bool BezierCurve::getRenderPolyline() const
{
	return m_renderPolyline;
}

void BezierCurve::setRenderPolyline(bool renderPolyline)
{
	m_renderPolyline = renderPolyline;
}

void BezierCurve::useCurveShaderProgram() const
{
	m_curveShaderProgram.use();
}

void BezierCurve::usePolylineShaderProgram() const
{
	m_polylineShaderProgram.use();
}

void BezierCurve::updateShaders() const
{
	useCurveShaderProgram();
	m_curveShaderProgram.setUniform("isSelected", isSelected());

	if (getRenderPolyline())
	{
		usePolylineShaderProgram();
		m_polylineShaderProgram.setUniform("isSelected", isSelected());
	}
}

std::vector<glm::vec3> BezierCurve::pointsToPolylineVertices(const std::vector<Point*> points)
{
	std::vector<glm::vec3> vertices{};
	for (const Point* point : points)
	{
		vertices.push_back(point->getPos());
	}
	return vertices;
}
