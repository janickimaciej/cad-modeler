#include "models/bezierCurves/c0BezierCurve.hpp"

#include "meshes/indicesMesh.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <memory>
#include <string>

C0BezierCurve::C0BezierCurve(const std::vector<Point*>& points,
	const SelfDestructCallback& selfDestructCallback) :
	CXBezierCurve{"C0 Bezier curve " + std::to_string(m_count++), points, selfDestructCallback}
{
	createCurveMesh();
	registerForNotifications(m_points);
}

void C0BezierCurve::addPoints(const std::vector<Point*>& points)
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

int C0BezierCurve::m_count = 0;

void C0BezierCurve::createCurveMesh()
{
	m_curveMesh = std::make_unique<IndicesMesh>(createVertices(m_points),
		createCurveIndices(m_points), GL_PATCHES, 4);
}

void C0BezierCurve::updateCurveMesh()
{
	m_curveMesh->update(createVertices(m_points));
	m_curveMesh->update(createCurveIndices(m_points));
}
