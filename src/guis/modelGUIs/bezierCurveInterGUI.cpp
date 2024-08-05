#include "guis/modelGUIs/bezierCurveInterGUI.hpp"

#include "models/bezierCurveInter.hpp"

#include <imgui/imgui.h>

#include <string>
#include <vector>

BezierCurveInterGUI::BezierCurveInterGUI(BezierCurveInter& curve) :
	ModelGUI{curve},
	m_curve{curve}
{ }

void BezierCurveInterGUI::update()
{
	bool renderPolyline = m_curve.getRenderPolyline();
	bool prevRenderPolyline = renderPolyline;
	ImGui::Checkbox(("render polyline" + suffix()).c_str(), &renderPolyline);
	if (renderPolyline != prevRenderPolyline)
	{
		m_curve.setRenderPolyline(renderPolyline);
	}
	
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
