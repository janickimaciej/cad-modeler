#include "guis/addGregorySurfacePanel.hpp"

#include <imgui/imgui.h>

AddGregorySurfacePanel::AddGregorySurfacePanel(Scene& scene, const Callback& callback) :
	m_scene{scene},
	m_callback{callback}
{ }

void AddGregorySurfacePanel::start()
{
	m_scene.startAddingGregoryPatch();
}

void AddGregorySurfacePanel::reset()
{
	m_scene.stopAddingGregoryPatch();
	m_chosenPatches = 0;
}

void AddGregorySurfacePanel::update()
{
	std::optional<int> clickedPatch = std::nullopt;

	updateList(clickedPatch);
	updateButton(clickedPatch);
}

void AddGregorySurfacePanel::updateList(std::optional<int>& clickedPatch)
{	
	ImGuiTreeNodeFlags globalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	for (int i = 0; i < m_scene.getModelCount(ModelType::bezierPatch); ++i)
	{
		ImGuiTreeNodeFlags flags = globalFlags;
		if (m_scene.isModelSelected(i, ModelType::bezierPatch))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			clickedPatch = i;
		}
		ImGui::TreeNodeEx((m_scene.getModelName(i, ModelType::bezierPatch) +
			"##AddGregorySurfacePanelPatches" +
			m_scene.getModelOriginalName(i, ModelType::bezierPatch)).c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			m_scene.selectModel(i, ModelType::bezierPatch);
		}
	}
}

void AddGregorySurfacePanel::updateButton(std::optional<int> clickedPatch)
{
	if (ImGui::Button(("Add patch (" + std::to_string(m_chosenPatches) +
		"/3)##AddGregorySurfacePanelApply").c_str()) && clickedPatch.has_value())
	{
		m_patches[m_chosenPatches] = *clickedPatch;
		++m_chosenPatches;
		m_scene.deselectAllModels();
		if (m_chosenPatches == 3)
		{
			m_scene.addGregorySurface(m_patches);
			m_callback();
		}
	}
}
