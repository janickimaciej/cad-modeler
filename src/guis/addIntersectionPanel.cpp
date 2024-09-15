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
	m_surfaceCount = 0;
	m_useCursor = false;
}

void AddIntersectionPanel::update()
{
	std::optional<ModelType> clickedType = std::nullopt;
	std::optional<int> clickedSurfaceIndex = std::nullopt;

	updateCheckbox();
	updateList(clickedType, clickedSurfaceIndex);
	updateButton(clickedType, clickedSurfaceIndex);
}

void AddIntersectionPanel::updateCheckbox()
{
	ImGui::Checkbox("Use cursor##AddIntersectionPanelCheckbox", &m_useCursor);
}

void AddIntersectionPanel::updateList(std::optional<ModelType>& clickedType,
	std::optional<int>& clickedSurfaceIndex)
{
	updateList(clickedType, clickedSurfaceIndex, ModelType::torus);
	updateList(clickedType, clickedSurfaceIndex, ModelType::bezierPatch);
	updateList(clickedType, clickedSurfaceIndex, ModelType::bezierSurfaceC0);
	updateList(clickedType, clickedSurfaceIndex, ModelType::bezierSurfaceC2);
}

void AddIntersectionPanel::updateList(std::optional<ModelType>& clickedType,
	std::optional<int>& clickedSurfaceIndex, ModelType type)
{
	ImGuiTreeNodeFlags globalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	for (int i = 0; i < m_scene.getModelCount(type); ++i)
	{
		ImGuiTreeNodeFlags flags = globalFlags;
		if (m_scene.isModelSelected(i, type))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			clickedType = type;
			clickedSurfaceIndex = i;
		}
		ImGui::TreeNodeEx((m_scene.getModelName(i, type) + "##AddIntersectionPanelList" +
			m_scene.getModelOriginalName(i, type)).c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			m_scene.selectModel(i, type);
		}
	}
}

void AddIntersectionPanel::updateButton(std::optional<ModelType> clickedType,
	std::optional<int> clickedSurfaceIndex)
{
	if (ImGui::Button(("Add surface (" + std::to_string(m_surfaceCount) +
		"/2)##AddIntersectionPanelButton").c_str()) && clickedType.has_value())
	{
		m_types[m_surfaceCount] = *clickedType;
		m_surfaceIndices[m_surfaceCount] = *clickedSurfaceIndex;
		++m_surfaceCount;
		m_scene.deselectAllModels();
		if (m_surfaceCount == 2)
		{
			m_scene.addIntersection(m_types, m_surfaceIndices, m_useCursor);
			m_callback();
		}
	}
}
