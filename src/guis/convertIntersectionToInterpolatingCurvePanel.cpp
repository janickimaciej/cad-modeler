#include "convertIntersectionToInterpolatingCurvePanel.hpp"

#include <imgui/imgui.h>

ConvertIntersectionToInterpolatingCurvePanel::ConvertIntersectionToInterpolatingCurvePanel(
	Scene& scene, const Callback& callback) :
	m_scene{scene},
	m_callback{callback}
{ }

void ConvertIntersectionToInterpolatingCurvePanel::reset()
{
	m_numberOfPoints = 50;
}

void ConvertIntersectionToInterpolatingCurvePanel::update()
{
	ImGui::Spacing();

	ImGui::InputInt("Number of points##ConvertIntersectionToInterpolatingCurvePanel",
		&m_numberOfPoints, 1, 1);
	m_numberOfPoints = std::max(m_numberOfPoints, 4);

	ImGui::Spacing();

	if (ImGui::Button("Convert"))
	{
		m_scene.convertIntersectionToInterpolatingCurve(m_numberOfPoints);
		m_callback();
	}
}
