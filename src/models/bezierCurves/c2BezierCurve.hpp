#pragma once

#include "meshes/mesh.hpp"
#include "models/bezierCurves/cxBezierCurve.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <memory>
#include <vector>

class C2BezierCurve : public CXBezierCurve
{
	friend class C2BezierCurveSerializer;

public:
	C2BezierCurve(const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const ShaderProgram& pointShaderProgram,
		const std::vector<Point*>& points, const SelfDestructCallback& selfDestructCallback,
		std::vector<std::unique_ptr<Point>>& bezierPoints);
	virtual ~C2BezierCurve() = default;
	virtual void render() const override;

	void addPoints(const std::vector<Point*>& points,
		std::vector<std::unique_ptr<Point>>& bezierPoints);
	virtual void deletePoint(int index) override;

private:
	static int m_count;

	const ShaderProgram& m_pointShaderProgram;

	std::unique_ptr<Mesh> m_bezierPolylineMesh{};

	std::vector<Point*> m_bezierPoints{};
	std::vector<std::shared_ptr<Point::MoveCallback>> m_bezierPointMoveNotifications{};
	bool m_blockNotifications = false;

	virtual void createCurveMesh() override;
	std::vector<std::unique_ptr<Point>> createBezierPoints();
	void createBezierPolylineMesh();
	std::vector<Point*> getAllBezierPoints() const;

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
	virtual void pointRereferenceNotification(const Point* point, Point* newPoint) override;
	void bezierPointMoveNotification(const Point* point);

	int getBezierPointIndex(const Point* point) const;
};
