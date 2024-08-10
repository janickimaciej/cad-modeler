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
#include <utility>
#include <vector>

class BezierCurveC2 : public BezierCurve
{
public:
	static std::pair<std::unique_ptr<BezierCurveC2>, std::vector<std::unique_ptr<Point>>>
		create(const ShaderProgram& curveShaderProgram, const ShaderProgram& polylineShaderProgram,
		const ShaderProgram& pointShaderProgram, const std::vector<Point*>& boorPoints);
	static std::vector<std::unique_ptr<Point>> createBezierPoints(
		const ShaderProgram& pointShaderProgram, const std::vector<Point*> boorPoints);
	static void updateBezierPoints(const std::vector<Point*>& bezierPoints,
		const std::vector<Point*> boorPoints);

	virtual void render() const override;

	virtual int pointCount() const override;
	std::vector<std::unique_ptr<Point>> addPoints(const std::vector<Point*>& points);
	virtual void deletePoint(int index) override;

	virtual std::string pointName(int index) const override;

private:
	static int m_count;
	
	const ShaderProgram& m_pointShaderProgram;

	std::unique_ptr<BezierCurveMesh> m_curveMesh{};
	std::unique_ptr<PolylineMesh> m_boorPolylineMesh{};
	std::unique_ptr<PolylineMesh> m_bezierPolylineMesh{};

	std::vector<Point*> m_boorPoints{};
	std::vector<Point*> m_bezierPoints{};
	std::vector<std::shared_ptr<Point::Callback>> m_moveNotificationsBoor{};
	std::vector<std::shared_ptr<Point::Callback>> m_destroyNotificationsBoor{};
	std::vector<std::shared_ptr<Point::Callback>> m_moveNotificationsBezier{};
	bool firstCall = true;

	BezierCurveC2(const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const ShaderProgram& pointShaderProgram,
		const std::vector<Point*>& boorPoints, const std::vector<Point*>& bezierPoints);

	void createCurveMesh();
	void createBoorPolylineMesh();
	void createBezierPolylineMesh();

	void updateWithBezierPoint(int index);
	void updateBezierPoints() const;
	void updateMeshes();

	void updatePos();
	void updateCurveMesh();
	void updateBoorPolylineMesh();
	void updateBezierPolylineMesh();

	void deleteBezierPoint(int index);

	void registerForNotificationsBoor(Point* point);
	void registerForNotificationsBoor(const std::vector<Point*>& points);
	void registerForNotificationsBezier(Point* point);
	void registerForNotificationsBezier(const std::vector<Point*>& points);

	void renderCurve() const;
	void renderBoorPolyline() const;
	void renderBezierPolyline() const;
	
	static std::vector<glm::vec3> pointsToCurveVertices(const std::vector<Point*> points);
};
