#include "meshes/bezierCurveInterMesh.hpp"

#include <glad/glad.h>

BezierCurveInterMesh::BezierCurveInterMesh(const std::vector<BezierCurveInterSegmentData>& segments)
{
	glGenVertexArrays(1, &m_VAO);
	createVBO(segments);
}

BezierCurveInterMesh::~BezierCurveInterMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void BezierCurveInterMesh::update(const std::vector<BezierCurveInterSegmentData>& segments)
{
	m_segmentCount = segments.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(segments.size() * sizeof(BezierCurveInterSegmentData)),
		segments.data(), GL_DYNAMIC_DRAW);
}

void BezierCurveInterMesh::render() const
{
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(m_segmentCount));
	glBindVertexArray(0);
}

void BezierCurveInterMesh::createVBO(const std::vector<BezierCurveInterSegmentData>& segments)
{
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, a)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, b)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, c)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, d)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, nextPoint)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(BezierCurveInterSegmentData),
		reinterpret_cast<void*>(offsetof(BezierCurveInterSegmentData, dt)));
	glEnableVertexAttribArray(5);
	glBindVertexArray(0);

	update(segments);
}
