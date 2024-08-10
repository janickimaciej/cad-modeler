#pragma once

#include "guis/modelGUIs/bezierCurveGUI.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

class BezierCurve : public Model
{
public:
	BezierCurve(const std::string& name, const ShaderProgram& curveShaderProgram,
		const ShaderProgram& polylineShaderProgram);

	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;
	virtual void setScreenPos(const glm::vec2&, const glm::mat4&, const glm::ivec2&) override;

	virtual int pointCount() const = 0;
	virtual void deletePoint(int index) = 0;

	virtual std::string pointName(int index) const = 0;

	bool getRenderPolyline() const;
	void setRenderPolyline(bool renderPolyline);

protected:
	void useCurveShaderProgram() const;
	void usePolylineShaderProgram() const;
	virtual void updateShaders() const override;

	static std::vector<glm::vec3> pointsToPolylineVertices(const std::vector<Point*> points);

private:
	const ShaderProgram& m_curveShaderProgram;
	const ShaderProgram& m_polylineShaderProgram;

	BezierCurveGUI m_gui{*this};

	bool m_renderPolyline = true;
};
