#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "guis/modelGUIs/bezierCurveC0GUI.hpp"
#include "meshes/bezierCurveMesh.hpp"
#include "meshes/polylineMesh.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class BezierCurveC0 : public Model
{
public:
	BezierCurveC0(const ShaderProgram& bezierCurveShaderProgram,
		const ShaderProgram& bezierCurvePolylineShaderProgram, const std::vector<Point*>& points);
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
	BezierCurveC0GUI m_gui;

	std::unique_ptr<BezierCurveMesh> m_curveMesh{};
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

	static std::vector<glm::vec3> pointsToCurveVertices(const std::vector<Point*> points);
	static std::vector<glm::vec3> pointsToPolylineVertices(const std::vector<Point*> points);
};
