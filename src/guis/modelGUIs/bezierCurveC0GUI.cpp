#include "guis/modelGUIs/bezierCurveC0GUI.hpp"

#include "models/bezierCurveC0.hpp"

#include <imgui/imgui.h>

#include <string>
#include <vector>

BezierCurveC0GUI::BezierCurveC0GUI(BezierCurveC0& curve) :
	ModelGUI{curve},
	m_curve{curve}
{ }

void BezierCurveC0GUI::update()
{
	bool renderPolyline = m_curve.getRenderPolyline();
	ImGui::Checkbox(("render polyline" + suffix()).c_str(), &renderPolyline);
	m_curve.setRenderPolyline(renderPolyline);

	std::vector<std::string> pointNames = m_curve.getPointNames();
	for (int i = 0; i < pointNames.size(); ++i)
	{
		ImGui::Text(pointNames[i].c_str());
		ImGui::SameLine(ImGui::GetWindowWidth() - 35);
		if (ImGui::SmallButton(("X" + suffix() + std::to_string(i)).c_str()))
		{
			m_curve.deletePoint(i);
		}
	}
}
