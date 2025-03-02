#include "models/bezierCurves/bezierCurveC0.hpp"

#include "meshes/indicesMesh.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <memory>
#include <string>

BezierCurveC0::BezierCurveC0(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
	const SelfDestructCallback& selfDestructCallback) :
	BezierCurveCX{"C0 Bezier curve " + std::to_string(m_count++), curveShaderProgram,
		polylineShaderProgram, points, selfDestructCallback}
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
	m_curveMesh = std::make_unique<IndicesMesh>(createVertices(m_points),
		createCurveIndices(m_points), GL_PATCHES, 4);
}

void BezierCurveC0::updateCurveMesh()
{
	m_curveMesh->update(createVertices(m_points));
	m_curveMesh->update(createCurveIndices(m_points));
}
