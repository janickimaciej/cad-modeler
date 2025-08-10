#include "meshes/interpolatingBezierCurveMesh.hpp"

#include <glad/glad.h>

InterpolatingBezierCurveMesh::InterpolatingBezierCurveMesh(
	const std::vector<InterpolatingBezierCurveSegmentData>& segments)
{
	glGenVertexArrays(1, &m_VAO);
	createVBO(segments);
}

InterpolatingBezierCurveMesh::~InterpolatingBezierCurveMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void InterpolatingBezierCurveMesh::update(
	const std::vector<InterpolatingBezierCurveSegmentData>& segments)
{
	m_segmentCount = segments.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(segments.size() * sizeof(InterpolatingBezierCurveSegmentData)),
		segments.data(), GL_DYNAMIC_DRAW);
}

void InterpolatingBezierCurveMesh::render() const
{
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(m_segmentCount));
	glBindVertexArray(0);
}

void InterpolatingBezierCurveMesh::createVBO(
	const std::vector<InterpolatingBezierCurveSegmentData>& segments)
{
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(InterpolatingBezierCurveSegmentData),
		reinterpret_cast<void*>(offsetof(InterpolatingBezierCurveSegmentData, a)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(InterpolatingBezierCurveSegmentData),
		reinterpret_cast<void*>(offsetof(InterpolatingBezierCurveSegmentData, b)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InterpolatingBezierCurveSegmentData),
		reinterpret_cast<void*>(offsetof(InterpolatingBezierCurveSegmentData, c)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(InterpolatingBezierCurveSegmentData),
		reinterpret_cast<void*>(offsetof(InterpolatingBezierCurveSegmentData, d)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InterpolatingBezierCurveSegmentData),
		reinterpret_cast<void*>(offsetof(InterpolatingBezierCurveSegmentData, nextPoint)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(InterpolatingBezierCurveSegmentData),
		reinterpret_cast<void*>(offsetof(InterpolatingBezierCurveSegmentData, dt)));
	glEnableVertexAttribArray(5);
	glBindVertexArray(0);

	update(segments);
}
