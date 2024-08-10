#pragma once

#include "meshes/polylineMesh.hpp"
#include "models/bezierCurves/bezierCurveCX.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <memory>
#include <utility>
#include <vector>

class BezierCurveC2 : public BezierCurveCX
{
public:
	static std::pair<std::unique_ptr<BezierCurveC2>, std::vector<std::unique_ptr<Point>>>
		create(const ShaderProgram& curveShaderProgram, const ShaderProgram& polylineShaderProgram,
		const ShaderProgram& pointShaderProgram, const std::vector<Point*>& points);
	static std::vector<std::unique_ptr<Point>> createBezierPoints(
		const ShaderProgram& pointShaderProgram, const std::vector<Point*> points);
	static void updateBezierPoints(const std::vector<Point*>& bezierPoints,
		const std::vector<Point*> points);

	virtual void render() const override;

	std::vector<std::unique_ptr<Point>> addPoints(const std::vector<Point*>& points);
	virtual void deletePoint(int index) override;

private:
	static int m_count;
	
	const ShaderProgram& m_pointShaderProgram;

	std::unique_ptr<PolylineMesh> m_bezierPolylineMesh{};

	std::vector<Point*> m_bezierPoints{};
	std::vector<std::shared_ptr<Point::Callback>> m_bezierPointMoveNotifications{};
	bool m_firstCall = true;

	BezierCurveC2(const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const ShaderProgram& pointShaderProgram,
		const std::vector<Point*>& points, const std::vector<Point*>& bezierPoints);

	virtual void createCurveMesh() override;
	void createBezierPolylineMesh();

	void updateWithBezierPoint(int index);
	void updateBezierPoints() const;
	virtual void updateGeometry() override;
	virtual void updateCurveMesh() override;
	void updateBezierPolylineMesh();

	void renderBezierPolyline() const;

	void deleteBezierPoint(int index);
	
	void registerForNotificationsBezier(const std::vector<Point*>& points);
	void registerForNotificationsBezier(Point* point);
	virtual void pointMoveNotification() override;
	virtual void pointDestroyNotification(const Point* point) override;
	void bezierPointMoveNotification(const Point* point);
};
