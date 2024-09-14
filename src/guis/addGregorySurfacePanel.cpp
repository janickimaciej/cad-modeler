#include "guis/addGregorySurfacePanel.hpp"

#include "models/modelType.hpp"

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
	m_patchCount = 0;
}

void AddGregorySurfacePanel::update()
{
	std::optional<int> clickedPatchIndex = std::nullopt;

	updateList(clickedPatchIndex);
	updateButton(clickedPatchIndex);
}

void AddGregorySurfacePanel::updateList(std::optional<int>& clickedPatchIndex)
{
	ImGuiTreeNodeFlags globalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	for (int i = 0; i < m_scene.getModelCount(ModelType::bezierPatch); ++i)
	{
		ImGuiTreeNodeFlags flags = globalFlags;
		if (m_scene.isModelSelected(i, ModelType::bezierPatch))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			clickedPatchIndex = i;
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

void AddGregorySurfacePanel::updateButton(std::optional<int> clickedPatchIndex)
{
	if (ImGui::Button(("Add patch (" + std::to_string(m_patchCount) +
		"/3)##AddGregorySurfacePanelApply").c_str()) && clickedPatchIndex.has_value())
	{
		m_patches[m_patchCount] = *clickedPatchIndex;
		++m_patchCount;
		m_scene.deselectAllModels();
		if (m_patchCount == 3)
		{
			m_scene.addGregorySurface(m_patches);
			m_callback();
		}
	}
}
