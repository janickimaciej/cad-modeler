#pragma once

#include "models/bezierCurves/bezierCurveInterSegmentData.hpp"

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

class BezierCurveInterMesh
{
public:
	BezierCurveInterMesh(const std::vector<BezierCurveInterSegmentData>& segments);
	~BezierCurveInterMesh();
	void update(const std::vector<BezierCurveInterSegmentData>& segments);
	void render() const;

private:
	std::size_t m_segmentCount{};
	unsigned int m_VBO{};
	unsigned int m_VAO{};

	void createVBO(const std::vector<BezierCurveInterSegmentData>& segments);
	void createVAO();
};
