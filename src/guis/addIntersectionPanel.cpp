#include "guis/addIntersectionPanel.hpp"

#include <imgui/imgui.h>

#include <string>

AddIntersectionPanel::AddIntersectionPanel(Scene& scene, const Callback& callback) :
	m_scene{scene},
	m_callback{callback}
{ }

void AddIntersectionPanel::reset()
{
	m_step = 0.01f;
	m_useCursor = false;
	m_surfaceCount = 0;
}

void AddIntersectionPanel::update()
{
	ImGui::Spacing();

	static constexpr float stepPrecision = 0.001f;
	static const std::string format = "%.3f";
	ImGui::InputFloat("step##AddIntersectionPanelFloat", &m_step, stepPrecision, stepPrecision,
		format.c_str());
	m_step = std::max(m_step, 0.001f);
	m_step = std::min(m_step, 0.1f);

	ImGui::Spacing();

	ImGui::Checkbox("use cursor##AddIntersectionPanelCheckbox", &m_useCursor);

	ImGui::Spacing();

	if (ImGui::Button(("Add surface (" + std::to_string(m_surfaceCount) +
		"/2)##AddIntersectionPanelButton").c_str()))
	{
		const Intersectable* surface = m_scene.getUniqueSelectedIntersectable();
		if (surface == nullptr)
		{
			return;
		}

		m_surfaces[m_surfaceCount] = surface;
		++m_surfaceCount;
		m_scene.deselectAllModels();
		if (m_surfaceCount == 2)
		{
			m_scene.addIntersectionCurve(m_surfaces, m_step, m_useCursor);
			m_callback();
		}
	}
}
