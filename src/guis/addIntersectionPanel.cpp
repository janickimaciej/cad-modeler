#include "guis/addIntersectionPanel.hpp"

#include <imgui/imgui.h>

#include <string>

AddIntersectionPanel::AddIntersectionPanel(Scene& scene, const Callback& callback) :
	m_scene{scene},
	m_callback{callback}
{ }

void AddIntersectionPanel::start()
{
	m_scene.startAddingIntersection();
}

void AddIntersectionPanel::reset()
{
	m_scene.stopAddingIntersection();
	m_surfaceCount = 0;
	m_useCursor = false;
}

void AddIntersectionPanel::update()
{
	ImGui::Spacing();

	ImGui::Checkbox("Use cursor##AddIntersectionPanelCheckbox", &m_useCursor);

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
			m_scene.addIntersection(m_surfaces, m_useCursor);
			m_callback();
		}
	}
}
