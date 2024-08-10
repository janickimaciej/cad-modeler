#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "meshes/bezierCurveMesh.hpp"
#include "meshes/polylineMesh.hpp"
#include "models/bezierCurves/bezierCurve.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class BezierCurveC0 : public BezierCurve
{
public:
	BezierCurveC0(const ShaderProgram& bezierCurveShaderProgram,
		const ShaderProgram& bezierCurvePolylineShaderProgram, const std::vector<Point*>& points);
	virtual void render() const override;

	virtual int pointCount() const override;
	void addPoints(const std::vector<Point*>& points);
	virtual void deletePoint(int index) override;

	virtual std::string pointName(int index) const override;

private:
	static int m_count;

	std::unique_ptr<BezierCurveMesh> m_curveMesh{};
	std::unique_ptr<PolylineMesh> m_polylineMesh{};

	std::vector<Point*> m_points{};
	std::vector<std::shared_ptr<Point::Callback>> m_moveNotifications{};
	std::vector<std::shared_ptr<Point::Callback>> m_destroyNotifications{};

	void createCurveMesh();
	void createPolylineMesh();

	void updateGeometry();

	void updatePos();
	void updateCurveMesh();
	void updatePolylineMesh();

	void registerForNotifications(Point* point);
	void registerForNotifications(const std::vector<Point*>& points);

	void renderCurve() const;
	void renderPolyline() const;

	static std::vector<glm::vec3> pointsToCurveVertices(const std::vector<Point*> points);
};
