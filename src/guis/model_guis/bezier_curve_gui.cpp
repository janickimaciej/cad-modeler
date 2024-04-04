#include "guis/model_guis/bezier_curve_gui.hpp"

#include "models/bezier_curve.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

BezierCurveGUI::BezierCurveGUI(BezierCurve& curve) :
	m_curve{curve}
{ }

void BezierCurveGUI::update()
{
	const std::string suffix = "##" + m_curve.getOriginalName();

	getValues();

	ImGui::Checkbox(("render polyline" + suffix).c_str(), &m_renderPolyline);

	for (int i = 0; i < m_pointNames.size(); ++i)
	{
		ImGui::Text(m_pointNames[i].c_str());
		ImGui::SameLine(ImGui::GetWindowWidth() - 25);
		if (ImGui::SmallButton(("X##" + suffix + std::to_string(i)).c_str()))
		{
			m_curve.deletePoint(i);
		}
	}

	setValues();
}

void BezierCurveGUI::getValues()
{
	m_renderPolyline = m_curve.getRenderPolyline();
	m_pointNames = m_curve.getPointNames();
}

void BezierCurveGUI::setValues()
{
	m_curve.setRenderPolyline(m_renderPolyline);
}
