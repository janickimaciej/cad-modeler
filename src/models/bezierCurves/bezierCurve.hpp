#pragma once

#include "guis/modelGUIs/bezierCurveGUI.hpp"
#include "meshes/mesh.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"
#include "shaderPrograms.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class BezierCurve : public Model
{
public:
	using SelfDestructCallback = std::function<void(const BezierCurve*)>;

	BezierCurve(const std::string& name, const std::vector<Point*>& points,
		const ShaderProgram& curveShaderProgram, const SelfDestructCallback& selfDestructCallback);
	virtual ~BezierCurve() = default;
	virtual void render() const override;

	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;

	int pointCount() const;
	virtual void deletePoint(int index);

	std::string pointName(int index) const;

	bool getRenderPolyline() const;
	void setRenderPolyline(bool renderPolyline);

protected:
	std::vector<Point*> m_points{};

	void useCurveShaderProgram() const;
	void usePolylineShaderProgram() const;
	virtual void updateShaders() const override;

	virtual void createCurveMesh() = 0;
	void createPolylineMesh();

	virtual void updateGeometry();
	void updatePos();
	virtual void updateCurveMesh() = 0;
	void updatePolylineMesh();

	virtual void renderCurve() const = 0;
	void renderPolyline() const;

	void registerForNotifications(const std::vector<Point*>& points);
	void registerForNotifications(Point* point);
	virtual void pointMoveNotification();
	virtual void pointDestroyNotification(const Point* point);
	virtual void pointRereferenceNotification(const Point* point, Point* newPoint);

	static std::vector<glm::vec3> createVertices(const std::vector<Point*>& points);

private:
	const ShaderProgram& m_curveShaderProgram;
	const ShaderProgram& m_polylineShaderProgram = *ShaderPrograms::polyline;

	BezierCurveGUI m_gui{*this};

	std::unique_ptr<Mesh> m_polylineMesh{};
	bool m_renderPolyline = false;

	std::vector<std::shared_ptr<Point::MoveCallback>> m_pointMoveNotifications{};
	std::vector<std::shared_ptr<Point::DestroyCallback>> m_pointDestroyNotifications{};
	std::vector<std::shared_ptr<Point::RereferenceCallback>> m_pointRereferenceNotifications{};

	SelfDestructCallback m_selfDestructCallback;

	int getPointIndex(const Point* point) const;
};
