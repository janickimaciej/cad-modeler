#include "models/bezierCurves/bezierCurveC0.hpp"

#include <algorithm>
#include <memory>
#include <string>

BezierCurveC0::BezierCurveC0(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points) :
	BezierCurveCX{"BezierCurveC0 " + std::to_string(m_count++), curveShaderProgram,
		polylineShaderProgram, points}
{
	createCurveMesh();
	registerForNotifications(m_points);
}

void BezierCurveC0::addPoints(const std::vector<Point*>& points)
{
	for (Point* point : points)
	{
		if (std::find(m_points.begin(), m_points.end(), point) == m_points.end())
		{
			m_points.push_back(point);
			registerForNotifications(point);
		}
	}
	updateGeometry();
}

int BezierCurveC0::m_count = 0;

void BezierCurveC0::createCurveMesh()
{
	m_curveMesh = std::make_unique<BezierCurveMesh>(pointsToCurveVertices(m_points));
}

void BezierCurveC0::updateCurveMesh()
{
	m_curveMesh->update(pointsToCurveVertices(m_points));
}
