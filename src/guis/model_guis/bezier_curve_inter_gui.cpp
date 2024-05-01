#include "guis/model_guis/bezier_curve_inter_gui.hpp"

#include "models/bezier_curve_inter.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

BezierCurveInterGUI::BezierCurveInterGUI(BezierCurveInter& curve) :
	m_curve{curve}
{ }

void BezierCurveInterGUI::update()
{
	const std::string suffix = "##" + m_curve.getOriginalName();

	getValues();

	ImGui::Checkbox(("render polyline" + suffix).c_str(), &m_renderPolyline);

	for (int i = 0; i < m_pointNames.size(); ++i)
	{
		ImGui::Text(m_pointNames[i].c_str());
		ImGui::SameLine(ImGui::GetWindowWidth() - 35);
		if (ImGui::SmallButton(("X##" + suffix + std::to_string(i)).c_str()))
		{
			m_curve.deletePoint(i);
		}
	}

	setValues();
}

void BezierCurveInterGUI::getValues()
{
	m_renderPolyline = m_curve.getRenderPolyline();
	m_pointNames = m_curve.getPointNames();
}

void BezierCurveInterGUI::setValues()
{
	m_curve.setRenderPolyline(m_renderPolyline);
}
