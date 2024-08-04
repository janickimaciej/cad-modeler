#include "guis/leftPanel.hpp"

#include <imgui/imgui.h>

#include <optional>

LeftPanel::LeftPanel(Scene& scene, const glm::ivec2& windowSize) :
	m_scene{scene},
	m_windowSize{windowSize}
{ }

void LeftPanel::update(GUIMode mode)
{
	ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
	static constexpr int mainGUIWidth = 250;
	static constexpr int mainGUIHeight = 1000;
	ImGui::SetNextWindowSize({mainGUIWidth, mainGUIHeight}, ImGuiCond_Always);
	ImGui::Begin("mainGUI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBackground);
	ImGui::PushItemWidth(100);

	updateCamera();
	separator();
	updateCursor();
	separator();
	updateActiveModelsCenter();
	separator();
	updateButtons();
	separator();
	updateModelList(mode);

	ImGui::PopItemWidth();
	ImGui::End();
}

void LeftPanel::updateCamera()
{
	ImGui::Text("Camera");
	ImGui::Spacing();
	
	ImGui::PushItemWidth(120);
	
	m_cameraType = cameraTypeLabels[static_cast<int>(m_scene.getCameraType())];
	if (ImGui::BeginCombo("##cameraType", m_cameraType.c_str()))
	{
		for (int cameraTypeIndex = 0; cameraTypeIndex < cameraTypeCount; ++cameraTypeIndex)
		{
			bool isSelected = m_cameraType == cameraTypeLabels[cameraTypeIndex];
			if (ImGui::Selectable(cameraTypeLabels[cameraTypeIndex].c_str(), isSelected))
			{
				m_cameraType = cameraTypeLabels[cameraTypeIndex];
			}
		}
		ImGui::EndCombo();
	}
	int cameraTypeIndex =
		static_cast<int>(std::find(cameraTypeLabels.begin(), cameraTypeLabels.end(), m_cameraType) -
		cameraTypeLabels.begin());
	m_scene.setCameraType(static_cast<CameraType>(cameraTypeIndex));
	
	ImGui::PopItemWidth();

	m_scene.getActiveCamera().getGUI().update();
}

void LeftPanel::updateCursor()
{
	ImGui::Text("Cursor");
	ImGui::Spacing();
	m_scene.getCursor().getGUI().update(m_scene.getActiveCamera().getMatrix(), m_windowSize);
}

void LeftPanel::updateActiveModelsCenter()
{
	ImGui::Text("Active models");
	ImGui::Spacing();
	m_scene.getActiveModelsCenter().getGUI().update();
}

void LeftPanel::updateButtons()
{
	if (ImGui::Button("Add point"))
	{
		m_scene.addPoint();
	}

	ImGui::Spacing();

	if (ImGui::Button("Add torus"))
	{
		m_scene.addTorus();
	}

	ImGui::Spacing();

	if (ImGui::Button("Add bezier curve C0"))
	{
		m_scene.addBezierCurveC0();
	}

	ImGui::Spacing();

	if (ImGui::Button("Add bezier curve C2"))
	{
		m_scene.addBezierCurveC2();
	}

	ImGui::Spacing();

	if (ImGui::Button("Add bezier curve inter"))
	{
		m_scene.addBezierCurveInter();
	}

	ImGui::Spacing();

	if (ImGui::Button("Add points to curve"))
	{
		m_scene.addActivePointsToCurve();
	}
}

void LeftPanel::updateModelList(GUIMode mode)
{
	ImGui::Text("Model list");

	static constexpr ImGuiTreeNodeFlags globalFlags =
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	std::optional<int> clickedId{};
	for (int i = 0; i < m_scene.getModelCount(); ++i)
	{
		if (m_scene.isModelVirtual(i))
		{
			continue;
		}

		ImGuiTreeNodeFlags flags = globalFlags;
		if (m_scene.isModelActive(i)) flags |= ImGuiTreeNodeFlags_Selected;

		bool isOpen = ImGui::TreeNodeEx(
			(m_scene.getModelName(i) + "##modelList" + std::to_string(i)).c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			clickedId = i;
		}
		if (isOpen)
		{
			ImGui::Spacing();
			m_scene.updateModelGUI(i);
			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	if (clickedId.has_value() && mode == GUIMode::none)
	{
		if (ImGui::GetIO().KeyCtrl)
		{
			m_scene.setModelIsActive(*clickedId, !m_scene.isModelActive(*clickedId));
		}
		else
		{
			for (int i = 0; i < m_scene.getModelCount(); ++i)
			{
				m_scene.setModelIsActive(i, i == *clickedId);
			}
		}
	}
}

void LeftPanel::separator()
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}
