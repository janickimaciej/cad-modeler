#include "guis/addGregorySurfacePanel.hpp"

#include "models/modelType.hpp"

#include <imgui/imgui.h>

AddGregorySurfacePanel::AddGregorySurfacePanel(Scene& scene, const Callback& callback) :
	m_scene{scene},
	m_callback{callback}
{ }

void AddGregorySurfacePanel::reset()
{
	m_patchCount = 0;
}

void AddGregorySurfacePanel::update()
{
	ImGui::Spacing();

	if (ImGui::Button(("Add patch (" + std::to_string(m_patchCount) +
		"/3)##AddGregorySurfacePanelButton").c_str()))
	{
		BezierPatch* patch = m_scene.getUniqueSelectedBezierPatch();
		if (patch == nullptr)
		{
			return;
		}

		m_patches[m_patchCount] = patch;
		++m_patchCount;
		m_scene.deselectAllModels();
		if (m_patchCount == 3)
		{
			m_scene.addGregorySurface(m_patches);
			m_callback();
		}
	}
}
