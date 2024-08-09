#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "guis/modelGUIs/bezierCurveInterGUI.hpp"
#include "meshes/bezierCurveInterMesh.hpp"
#include "meshes/polylineMesh.hpp"
#include "models/bezierCurveInterSegmentData.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class BezierCurveInter : public Model
{
public:
	BezierCurveInter(const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram, const std::vector<Point*>& points);
	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	int getPointCount() const;
	void addPoints(const std::vector<Point*>& points);
	void deletePoint(int index);

	std::vector<std::string> getPointNames() const;

	bool getRenderPolyline() const;
	void setRenderPolyline(bool renderPolyline);

private:
	static int m_count;
	
	const ShaderProgram& m_curveShaderProgram;
	const ShaderProgram& m_polylineShaderProgram;
	BezierCurveInterGUI m_gui;

	std::unique_ptr<BezierCurveInterMesh> m_curveMesh{};
	std::unique_ptr<PolylineMesh> m_polylineMesh{};

	std::vector<Point*> m_points{};
	std::vector<std::shared_ptr<Point::Callback>> m_moveNotifications{};
	std::vector<std::shared_ptr<Point::Callback>> m_destroyNotifications{};

	bool m_renderPolyline = true;

	void createCurveMesh();
	void createPolylineMesh();

	virtual void updateShaders() const override;
	void updateGeometry();

	void updatePos();
	void updateCurveMesh();
	void updatePolylineMesh();
	
	void registerForNotifications(Point* point);
	void registerForNotifications(const std::vector<Point*>& points);

	void renderCurve() const;
	void renderPolyline() const;

	static std::vector<BezierCurveInterSegmentData> pointsToCurveSegments(
		const std::vector<Point*> points);
	static std::vector<glm::vec3> pointsToPolylineVertices(const std::vector<Point*> points);
};
