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
	std::optional<ModelType> clickedType = std::nullopt;
	std::optional<int> clickedSurface = std::nullopt;
	std::optional<int> clickedPatch = std::nullopt;

	update(ModelType::bezierSurfaceC0, clickedType, clickedSurface, clickedPatch);
	update(ModelType::bezierSurfaceC2, clickedType, clickedSurface, clickedPatch);

	if (ImGui::Button(("Add patch (" + std::to_string(m_chosenPatches) +
		"/3)##AddGregorySurfacePanelApply").c_str()) && clickedType.has_value())
	{
		m_types[m_chosenPatches] = *clickedType;
		m_surfaces[m_chosenPatches] = *clickedSurface;
		m_patches[m_chosenPatches] = *clickedPatch;
		++m_chosenPatches;
		m_scene.deselectPatch();
		if (m_chosenPatches == 3)
		{
			m_scene.addGregorySurface(m_types, m_surfaces, m_patches);
			m_callback();
		}
	}
}

void AddGregorySurfacePanel::update(ModelType type, std::optional<ModelType>& clickedType,
	std::optional<int>& clickedSurface, std::optional<int>& clickedPatch)
{
	static constexpr ImGuiTreeNodeFlags surfaceFlags =
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	for (int surface = 0; surface < m_scene.getModelCount(type); ++surface)
	{
		bool isOpen = ImGui::TreeNodeEx((m_scene.getModelName(surface, type) +
			"##AddGregorySurfacePanelSurfaces" +
			m_scene.getModelOriginalName(surface, type)).c_str(), surfaceFlags);
		if (isOpen)
		{
			for (int patch = 0; patch < m_scene.getPatchCount(type, surface); ++patch)
			{
				ImGuiTreeNodeFlags patchFlags =
					ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (m_scene.isPatchSelected(type, surface, patch))
				{
					patchFlags |= ImGuiTreeNodeFlags_Selected;
					clickedType = type;
					clickedSurface = surface;
					clickedPatch = patch;
				}
				ImGui::TreeNodeEx(("Patch " + std::to_string(patch) +
					"##AddGregorySurfacePanelPatches" +
					m_scene.getModelOriginalName(surface, type)).c_str(), patchFlags);
				if (ImGui::IsItemClicked())
				{
					m_scene.selectPatch(type, surface, patch);
				}
			}
			ImGui::TreePop();
			ImGui::Spacing();
		}
	}
}
