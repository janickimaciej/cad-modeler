#include "guis/addIntersectionPanel.hpp"

#include <imgui/imgui.h>

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
	m_chosenSurfaces = 0;
}

void AddIntersectionPanel::update()
{
	std::optional<ModelType> clickedType = std::nullopt;
	std::optional<int> clickedSurface = std::nullopt;

	updateList(clickedType, clickedSurface);
	updateButton(clickedType, clickedSurface);
}

void AddIntersectionPanel::updateList(std::optional<ModelType>& clickedType,
	std::optional<int>& clickedSurface)
{
	updateList(clickedType, clickedSurface, ModelType::torus);
	updateList(clickedType, clickedSurface, ModelType::bezierPatch);
	updateList(clickedType, clickedSurface, ModelType::bezierSurfaceC0);
	updateList(clickedType, clickedSurface, ModelType::bezierSurfaceC2);
}

void AddIntersectionPanel::updateList(std::optional<ModelType>& clickedType,
	std::optional<int>& clickedSurface, ModelType type)
{
	ImGuiTreeNodeFlags globalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	for (int i = 0; i < m_scene.getModelCount(type); ++i)
	{
		ImGuiTreeNodeFlags flags = globalFlags;
		if (m_scene.isModelSelected(i, type))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			clickedType = type;
			clickedSurface = i;
		}
		ImGui::TreeNodeEx((m_scene.getModelName(i, type) + "##AddIntersectionPanelSurfaces" +
			m_scene.getModelOriginalName(i, type)).c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			m_scene.selectModel(i, type);
		}
	}
}

void AddIntersectionPanel::updateButton(std::optional<ModelType> clickedType,
	std::optional<int> clickedSurface)
{
	if (ImGui::Button(("Add patch (" + std::to_string(m_chosenSurfaces) +
		"/2)##AddIntersectionPanelApply").c_str()) && clickedType.has_value())
	{
		m_types[m_chosenSurfaces] = *clickedType;
		m_surfaces[m_chosenSurfaces] = *clickedSurface;
		++m_chosenSurfaces;
		m_scene.deselectAllModels();
		if (m_chosenSurfaces == 2)
		{
			m_scene.addIntersection(m_types, m_surfaces);
			m_callback();
		}
	}
}
