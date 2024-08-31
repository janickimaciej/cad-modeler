#include "models/bezierCurves/bezierCurveInter.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>

BezierCurveInter::BezierCurveInter(const ShaderProgram& curveShaderProgram,
	const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points,
	const SelfDestructCallback& selfDestructCallback) :
	BezierCurve{"Interpolating Bezier Curve " + std::to_string(m_count++), curveShaderProgram,
		polylineShaderProgram, points, selfDestructCallback}
{
	createCurveMesh();
	registerForNotifications(m_points);
}

void BezierCurveInter::addPoints(const std::vector<Point*>& points)
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

int BezierCurveInter::m_count = 0;

void BezierCurveInter::createCurveMesh()
{
	m_curveMesh = std::make_unique<BezierCurveInterMesh>(createCurveSegments());
}

void BezierCurveInter::updateCurveMesh()
{
	m_curveMesh->update(createCurveSegments());
}

void BezierCurveInter::renderCurve() const
{
	if (m_points.size() >= 3)
	{
		useCurveShaderProgram();
		m_curveMesh->render();
	}
}

std::vector<BezierCurveInterSegmentData> BezierCurveInter::createCurveSegments() const
{
	std::vector<BezierCurveInterSegmentData> segments{};
	if (m_points.size() >= 3)
	{
		std::size_t n = m_points.size() - 1;
		std::vector<float> dt(n);
		std::vector<glm::vec3> a(n);
		std::vector<glm::vec3> b(n);
		std::vector<glm::vec3> c(n);
		std::vector<glm::vec3> d(n);
		std::vector<float> alpha(n);
		std::vector<float> beta(n);
		std::vector<float> betap(n);
		std::vector<glm::vec3> R(n);
		std::vector<glm::vec3> Rp(n);

		for (std::size_t i = 0; i < n; ++i)
		{
			dt[i] = glm::length(m_points[i + 1]->getPos() - m_points[i]->getPos());
			a[i] = m_points[i]->getPos();
		}

		for (std::size_t i = 2; i < n; ++i)
		{
			alpha[i] = dt[i - 1] / (dt[i - 1] + dt[i]);
		}

		for (std::size_t i = 1; i < n - 1; ++i)
		{
			beta[i] = dt[i] / (dt[i - 1] + dt[i]);
		}

		for (std::size_t i = 1; i < n; ++i)
		{
			R[i] = 3.0f * ((m_points[i + 1]->getPos() - m_points[i]->getPos()) / dt[i] -
				(m_points[i]->getPos() - m_points[i - 1]->getPos()) / dt[i - 1]) /
				(dt[i - 1] + dt[i]);
		}

		betap[1] = beta[1] / 2;
		for (std::size_t i = 2; i < n - 1; ++i)
		{
			betap[i] = beta[i] / (2 - alpha[i] * betap[i - 1]);
		}

		Rp[1] = R[1] / 2.0f;
		for (std::size_t i = 2; i < n; ++i)
		{
			Rp[i] = (R[i] - alpha[i] * Rp[i - 1]) / (2 - alpha[i] * betap[i - 1]);
		}

		c[n - 1] = Rp[n - 1];
		for (std::size_t i = n - 2; i >= 1; --i)
		{
			c[i] = Rp[i] - betap[i] * c[i + 1];
		}
		c[0] = {0, 0, 0};

		for (std::size_t i = 0; i < n - 1; ++i)
		{
			d[i] = 2.0f * (c[i + 1] - c[i]) / (6 * dt[i]);
		}
		d[n - 1] = -2.0f * c[n - 1] / (6 * dt[n - 1]);

		for (std::size_t i = 0; i < n - 1; ++i)
		{
			b[i] = (a[i + 1] - a[i]) / dt[i] - (c[i] + d[i] * dt[i]) * dt[i];
		}
		b[n - 1] = (m_points[n]->getPos() - a[n - 1]) / dt[n - 1] -
			(c[n - 1] + d[n - 1] * dt[n - 1]) * dt[n - 1];

		for (std::size_t i = 0; i < n; ++i)
		{
			segments.push_back
			(
				{
					a[i],
					b[i],
					c[i],
					d[i],
					m_points[i + 1]->getPos(),
					dt[i]
				}
			);
		}
	}
	return segments;
}
