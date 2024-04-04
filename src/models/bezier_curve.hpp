#pragma once

#include "guis/model_guis/model_gui.hpp"
#include "guis/model_guis/bezier_curve_gui.hpp"
#include "models/model.hpp"
#include "render_mode.hpp"
#include "scene.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Point;

class BezierCurve : public Model
{
public:
	BezierCurve(const Scene& scene, const ShaderProgram& bezierCurveShaderProgram,
		const ShaderProgram& bezierCurvePolylineShaderProgram, const std::vector<Point*>& points);
	virtual void render(RenderMode renderMode) const override;
	virtual ModelGUI& getGUI() override;

	virtual void setPosition(const glm::vec3& position) override;
	virtual void setScreenPosition(const glm::vec2& screenPosition) override;

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
	BezierCurveGUI m_gui;

	unsigned int m_VBO{};
	unsigned int m_VAO{};
	unsigned int m_VBOPolyline{};
	unsigned int m_VAOPolyline{};

	std::vector<Point*> m_points{};
	std::vector<std::shared_ptr<std::function<void(Point*)>>> m_notifications{};

	bool m_renderPolyline = true;

	void createMesh();
	void createPolylineMesh();

	virtual void updateShaders(RenderMode renderMode) const override;
	void updatePosition();
	void updateMesh();
	void updatePolylineMesh();

	void registerForNotifications(const std::vector<Point*>& points);
};
