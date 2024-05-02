#pragma once

#include "guis/model_guis/model_gui.hpp"
#include "guis/model_guis/bezier_curve_c2_gui.hpp"
#include "models/curve_base.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "render_mode.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Point;

class BezierCurveC2 : public Model
{
public:
	static std::pair<std::unique_ptr<BezierCurveC2>, std::vector<std::unique_ptr<Point>>>
		create(const ShaderProgram& bezierCurveShaderProgram,
		const ShaderProgram& bezierCurvePolylineShaderProgram,
		const ShaderProgram& pointShaderProgram, const std::vector<Point*>& boorPoints);
	static std::vector<std::unique_ptr<Point>> createBezierPoints(
		const ShaderProgram& pointShaderProgram, const std::vector<Point*> boorPoints);
	static void updateBezierPoints(const std::vector<Point*>& bezierPoints,
		const std::vector<Point*> boorPoints);

	virtual void render(RenderMode renderMode) const override;
	virtual void updateGUI() override;

	virtual void setPosition(const glm::vec3&) override;
	virtual void setScreenPosition(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	int getPointCount() const;
	std::vector<std::unique_ptr<Point>> addPoints(const std::vector<Point*>& points);
	void deleteBoorPoint(int index);

	std::vector<std::string> getPointNames() const;

	bool getRenderPolyline() const;
	void setRenderPolyline(bool renderPolyline);

private:
	static int m_count;
	int m_id{};
	
	const ShaderProgram& m_bezierCurveShaderProgram;
	const ShaderProgram& m_bezierCurvePolylineShaderProgram;
	const ShaderProgram& m_pointShaderProgram;
	BezierCurveC2GUI m_gui;

	unsigned int m_VBOCurve{};
	unsigned int m_VAOCurve{};
	unsigned int m_VBOBoorPolyline{};
	unsigned int m_VAOBoorPolyline{};
	unsigned int m_VBOBezierPolyline{};
	unsigned int m_VAOBezierPolyline{};

	std::vector<Point*> m_boorPoints{};
	std::vector<Point*> m_bezierPoints{};
	std::vector<std::shared_ptr<std::function<void(Point*)>>> m_moveNotificationsBoor{};
	std::vector<std::shared_ptr<std::function<void(Point*)>>> m_destroyNotificationsBoor{};
	std::vector<std::shared_ptr<std::function<void(Point*)>>> m_moveNotificationsBezier{};
	bool firstCall = true;

	CurveBase m_base = CurveBase::boor;
	bool m_renderPolyline = true;

	BezierCurveC2(const ShaderProgram& bezierCurveShaderProgram,
		const ShaderProgram& bezierCurvePolylineShaderProgram,
		const ShaderProgram& pointShaderProgram, const std::vector<Point*>& boorPoints,
		const std::vector<Point*>& bezierPoints);

	void createCurveMesh();
	void createBoorPolylineMesh();
	void createBezierPolylineMesh();

	virtual void updateShaders(RenderMode) const override;
	void updateWithBezierPoint(int index);
	void updateBezierPoints() const;
	void updateMeshes();

	void updatePosition();
	void updateCurveMesh();
	void updateBoorPolylineMesh();
	void updateBezierPolylineMesh();

	void deleteBezierPoint(int index);

	void registerForNotificationsBoor(const std::vector<Point*>& points);
	void registerForNotificationsBezier(const std::vector<Point*>& points);

	void renderCurve() const;
	void renderBoorPolyline() const;
	void renderBezierPolyline() const;
};
