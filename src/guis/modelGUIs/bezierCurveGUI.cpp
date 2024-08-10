#include "guis/modelGUIs/bezierCurveGUI.hpp"

#include "models/bezierCurves/bezierCurve.hpp"

#include <imgui/imgui.h>

BezierCurveGUI::BezierCurveGUI(BezierCurve& curve) :
	ModelGUI{curve},
	m_curve{curve}
{ }

void BezierCurveGUI::update()
{
	bool renderPolyline = m_curve.getRenderPolyline();
	bool prevRenderPolyline = renderPolyline;
	ImGui::Checkbox(("render polyline" + suffix()).c_str(), &renderPolyline);
	if (renderPolyline != prevRenderPolyline)
	{
		m_curve.setRenderPolyline(renderPolyline);
	}

	for (int i = 0; i < m_curve.pointCount(); ++i)
	{
		ImGui::Text(m_curve.pointName(i).c_str());
		ImGui::SameLine(ImGui::GetWindowWidth() - 35);
		if (ImGui::SmallButton(("X" + suffix() + std::to_string(i)).c_str()))
		{
			m_curve.deletePoint(i);
		}
	}
}
