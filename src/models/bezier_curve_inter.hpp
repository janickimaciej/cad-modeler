#pragma once

#include "guis/model_guis/model_gui.hpp"
#include "guis/model_guis/bezier_curve_inter_gui.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "render_mode.hpp"
#include "scene.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Point;

class BezierCurveInter : public Model
{
public:
	BezierCurveInter(const Scene& scene, const ShaderProgram& bezierCurveShaderProgram,
		const ShaderProgram& bezierCurvePolylineShaderProgram, const std::vector<Point*>& points);
	virtual void render(RenderMode renderMode) const override;
	virtual void updateGUI() override;

	virtual void setPosition(const glm::vec3&) override;
	virtual void setScreenPosition(const glm::vec2&) override;

	int getPointCount() const;
	void addPoints(const std::vector<Point*>& points);
	void deletePoint(int index);

	std::vector<std::string> getPointNames() const;

	bool getRenderPolyline() const;
	void setRenderPolyline(bool renderPolyline);

private:
	static int m_count;
	int m_id{};
	
	const ShaderProgram& m_bezierCurveShaderProgram;
	const ShaderProgram& m_bezierCurvePolylineShaderProgram;
	BezierCurveInterGUI m_gui;

	unsigned int m_VBOCurve{};
	unsigned int m_VAOCurve{};
	unsigned int m_VBOPolyline{};
	unsigned int m_VAOPolyline{};

	std::vector<Point*> m_points{};
	std::vector<std::shared_ptr<std::function<void(Point*)>>> m_moveNotifications{};
	std::vector<std::shared_ptr<std::function<void(Point*)>>> m_destroyNotifications{};

	bool m_renderPolyline = true;

	void createCurveMesh();
	void createPolylineMesh();

	virtual void updateShaders(RenderMode) const override;
	void updateGeometry();

	void updatePosition();
	void updateCurveMesh();
	void updatePolylineMesh();

	void registerForNotifications(const std::vector<Point*>& points);

	void renderCurve() const;
	void renderPolyline() const;
};
