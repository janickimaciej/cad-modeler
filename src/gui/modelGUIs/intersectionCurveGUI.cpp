#include "gui/modelGUIs/intersectionCurveGUI.hpp"

#include "models/intersectionCurve.hpp"

#include <imgui/imgui.h>

#include <string>

IntersectionCurveGUI::IntersectionCurveGUI(IntersectionCurve& curve) :
	ModelGUI{curve},
	m_curve{curve}
{ }

void IntersectionCurveGUI::update()
{
	ImGui::Text(("Points: " + std::to_string(m_curve.pointCount())).c_str());
}
