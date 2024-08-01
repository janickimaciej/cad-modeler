#include "guis/modelGUIs/bezierCurveC2GUI.hpp"

#include "models/bezierCurveC2.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

BezierCurveC2GUI::BezierCurveC2GUI(BezierCurveC2& curve) :
	m_curve{curve}
{ }

void BezierCurveC2GUI::update()
{
	const std::string suffix = "##" + m_curve.getOriginalName();

	getValues();

	ImGui::Checkbox(("render polyline" + suffix).c_str(), &m_renderPolyline);

	for (int i = 0; i < m_pointNames.size(); ++i)
	{
		ImGui::Text(m_pointNames[i].c_str());
		ImGui::SameLine(ImGui::GetWindowWidth() - 35);
		if (ImGui::SmallButton(("X" + suffix + std::to_string(i)).c_str()))
		{
			m_curve.deleteBoorPoint(i);
		}
	}

	setValues();
}

void BezierCurveC2GUI::getValues()
{
	m_renderPolyline = m_curve.getRenderPolyline();
	m_pointNames = m_curve.getPointNames();
}

void BezierCurveC2GUI::setValues()
{
	m_curve.setRenderPolyline(m_renderPolyline);
}
