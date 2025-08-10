#pragma once

#include "models/bezierCurves/interpolatingBezierCurveSegmentData.hpp"

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class InterpolatingBezierCurveMesh
{
public:
	InterpolatingBezierCurveMesh(const std::vector<InterpolatingBezierCurveSegmentData>& segments);
	~InterpolatingBezierCurveMesh();
	void update(const std::vector<InterpolatingBezierCurveSegmentData>& segments);
	void render() const;

private:
	std::size_t m_segmentCount{};
	unsigned int m_VBO{};
	unsigned int m_VAO{};

	void createVBO(const std::vector<InterpolatingBezierCurveSegmentData>& segments);
};
