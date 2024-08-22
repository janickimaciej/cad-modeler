#include "guis/leftPanel.hpp"

#include <imgui/imgui.h>

#include <optional>

LeftPanel::LeftPanel(Scene& scene, const glm::ivec2& windowSize) :
	m_scene{scene},
	m_windowSize{windowSize},
	m_addBezierSurfaceC0Panel
	{
		[this] (int patchesU, int patchesV, float sizeU, float sizeV,
			BezierSurfaceWrapping wrapping)
		{
			m_scene.addBezierSurfaceC0(patchesU, patchesV, sizeU, sizeV, wrapping);
			m_addBezierSurfaceC0Panel.reset();
			m_addingBezierSurfaceC0 = false;
		}
	},
	m_addBezierSurfaceC2Panel
	{
		[this] (int patchesU, int patchesV, float sizeU, float sizeV,
			BezierSurfaceWrapping wrapping)
		{
			m_scene.addBezierSurfaceC2(patchesU, patchesV, sizeU, sizeV, wrapping);
			m_addBezierSurfaceC2Panel.reset();
			m_addingBezierSurfaceC2 = false;
		}
	}
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
	updateSelectedModelsCenter();
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
	
	m_cameraType = m_scene.getCameraType();
	if (ImGui::BeginCombo("##cameraType", cameraTypeLabels[static_cast<int>(m_cameraType)].c_str()))
	{
		for (int cameraType = 0; cameraType < cameraTypeCount; ++cameraType)
		{
			bool isSelected = static_cast<int>(m_cameraType) == cameraType;
			if (ImGui::Selectable(cameraTypeLabels[cameraType].c_str(), isSelected))
			{
				m_cameraType = static_cast<CameraType>(cameraType);
			}
		}
		ImGui::EndCombo();
	}
	m_scene.setCameraType(static_cast<CameraType>(m_cameraType));
	
	ImGui::PopItemWidth();

	m_scene.updateActiveCameraGUI();
}

void LeftPanel::updateCursor()
{
	ImGui::Text("Cursor");
	ImGui::Spacing();
	m_scene.updateCursorGUI();
}

void LeftPanel::updateSelectedModelsCenter()
{
	ImGui::Text("Selected models");
	ImGui::Spacing();
	m_scene.updateSelectedModelsCenterGUI();
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
		m_scene.addSelectedPointsToCurve();
	}

	ImGui::Spacing();
	
	if (ImGui::Button("Add bezier surface C0"))
	{
		m_addBezierSurfaceC0Panel.reset();
		m_addingBezierSurfaceC0 = !m_addingBezierSurfaceC0;

		m_addBezierSurfaceC2Panel.reset();
		m_addingBezierSurfaceC2 = false;
	}

	if (m_addingBezierSurfaceC0)
	{
		m_addBezierSurfaceC0Panel.update();
	}

	ImGui::Spacing();

	if (ImGui::Button("Add bezier surface C2"))
	{
		m_addBezierSurfaceC2Panel.reset();
		m_addingBezierSurfaceC2 = !m_addingBezierSurfaceC2;

		m_addBezierSurfaceC0Panel.reset();
		m_addingBezierSurfaceC0 = false;
	}

	if (m_addingBezierSurfaceC2)
	{
		m_addBezierSurfaceC2Panel.update();
	}
}

void LeftPanel::updateModelList(GUIMode mode)
{
	ImGui::Text("Model list");

	static constexpr ImGuiTreeNodeFlags globalFlags =
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	std::optional<int> clickedModel = std::nullopt;
	for (int i = 0; i < m_scene.getModelCount(); ++i)
	{
		if (m_scene.isModelVirtual(i))
		{
			continue;
		}

		ImGuiTreeNodeFlags flags = globalFlags;
		if (m_scene.isModelSelected(i))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = ImGui::TreeNodeEx(
			(m_scene.getModelName(i) + "##modelList" + std::to_string(i)).c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			clickedModel = i;
		}
		if (isOpen)
		{
			ImGui::Spacing();
			m_scene.updateModelGUI(i);
			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	if (clickedModel.has_value() && mode == GUIMode::none)
	{
		if (ImGui::GetIO().KeyCtrl)
		{
			m_scene.toggleModel(*clickedModel);
		}
		else
		{
			m_scene.deselectAllModels();
			m_scene.selectModel(*clickedModel);
		}
	}
}

void LeftPanel::separator()
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}
