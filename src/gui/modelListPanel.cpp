#include "gui/modelListPanel.hpp"

#include <imgui/imgui.h>

ModelListPanel::ModelListPanel(Scene& scene) :
	m_scene{scene}
{ }

void ModelListPanel::update(GUIMode mode)
{
	ImGui::Text("Model list");
	ImGui::Spacing();
	updateModelType();
	updateModelList(mode);
}

void ModelListPanel::updateModelType()
{
	ImGui::PushItemWidth(170);
	ImGui::SetNextWindowSize({170, 200});

	if (ImGui::BeginCombo("##modelType", modelTypeLabels[static_cast<int>(m_modelType)].c_str()))
	{
		for (int modelType = 0; modelType < modelTypeCount; ++modelType)
		{
			bool isSelected = modelType == static_cast<int>(m_modelType);
			if (ImGui::Selectable(modelTypeLabels[modelType].c_str(), isSelected))
			{
				m_modelType = static_cast<ModelType>(modelType);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::PopItemWidth();
}

void ModelListPanel::updateModelList(GUIMode mode)
{
	static constexpr ImGuiTreeNodeFlags globalFlags =
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	std::optional<int> clickedModel = std::nullopt;
	for (int i = 0; i < m_scene.getModelCount(m_modelType); ++i)
	{
		if (m_scene.isModelVirtual(i, m_modelType))
		{
			continue;
		}

		ImGuiTreeNodeFlags flags = globalFlags;
		if (m_scene.isModelSelected(i, m_modelType))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = ImGui::TreeNodeEx((m_scene.getModelName(i, m_modelType) + "##modelList" +
			m_scene.getModelOriginalName(i, m_modelType)).c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			clickedModel = i;
		}
		if (isOpen)
		{
			ImGui::Spacing();
			m_scene.updateModelGUI(i, m_modelType);
			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	if (clickedModel.has_value() && mode == GUIMode::none)
	{
		if (ImGui::GetIO().KeyCtrl)
		{
			m_scene.toggleModel(*clickedModel, m_modelType);
		}
		else
		{
			m_scene.deselectAllModels();
			m_scene.selectModel(*clickedModel, m_modelType);
		}
	}
}
