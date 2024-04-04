#include "guis/gui.hpp"

#include "camera_type.hpp"
#include "render_mode.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>
#include <optional>

GUI::GUI(GLFWwindow* window, Scene& scene, int windowWidth, int windowHeight) :
	m_scene{scene},
	m_windowWidth{windowWidth},
	m_windowHeight{windowHeight}
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void GUI::update()
{
	getValues();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (m_state == State::renaming)
	{
		constexpr int renamingWidth = 250;
		constexpr int renamingHeight = 35;
		ImGui::SetNextWindowPos({(m_windowWidth - renamingWidth + 50) / 2.0f,
			(m_windowHeight - renamingHeight) / 2.0f}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({renamingWidth, renamingHeight}, ImGuiCond_Always);
		ImGui::Begin("renaming", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		
		ImGui::PushItemWidth(renamingWidth - 17);
		if (m_focusFirstTime)
		{
			ImGui::SetKeyboardFocusHere();
			m_focusFirstTime = false;
		}
		ImGui::InputText("##renaming", m_name.data(), m_name.size());
		ImGui::PopItemWidth();

		ImGui::End();
	}

	if (m_state == State::rotatingX || m_state == State::rotatingY || m_state == State::rotatingZ)
	{
		constexpr int rotatingWidth = 100;
		constexpr int rotatingHeight = 35;
		ImGui::SetNextWindowPos({(m_windowWidth - rotatingWidth + 50) / 2.0f,
			(m_windowHeight - rotatingHeight) / 2.0f}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({rotatingWidth, rotatingHeight}, ImGuiCond_Always);
		ImGui::Begin("rotating", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		
		ImGui::PushItemWidth(rotatingWidth - 13);
		if (m_focusFirstTime)
		{
			ImGui::SetKeyboardFocusHere();
			m_focusFirstTime = false;
		}
		ImGui::InputFloat("##rotating", &m_rotationDeg, 1.0f, 1.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::End();
	}

	if (m_state == State::scalingX || m_state == State::scalingY || m_state == State::scalingZ)
	{
		constexpr int scalingWidth = 100;
		constexpr int scalingHeight = 35;
		ImGui::SetNextWindowPos({(m_windowWidth - scalingWidth + 50) / 2.0f,
			(m_windowHeight - scalingHeight) / 2.0f}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({scalingWidth, scalingHeight}, ImGuiCond_Always);
		ImGui::Begin("rotating", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		
		ImGui::PushItemWidth(scalingWidth - 13);
		if (m_focusFirstTime)
		{
			ImGui::SetKeyboardFocusHere();
			m_focusFirstTime = false;
		}
		ImGui::InputFloat("##scaling", &m_scale, 0.1f, 0.1f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::End();
	}

	ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
	constexpr int mainGUIWidth = 250;
	constexpr int mainGUIHeight = 1000;
	ImGui::SetNextWindowSize({mainGUIWidth, mainGUIHeight}, ImGuiCond_Always);
	ImGui::Begin("mainGUI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBackground);

	ImGui::PushItemWidth(150);

	renderMode();

	cameraType();
	
	separator();
	
	ImGui::PopItemWidth();
	ImGui::PushItemWidth(100);

	camera();

	separator();

	cursor();

	separator();

	activeModelsCenter();

	separator();

	buttons();

	separator();

	modelList();

	ImGui::PopItemWidth();
	ImGui::End();

	setValues();
}

void GUI::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::startRenaming()
{
	if (m_state == State::none)
	{
		m_uniqueActiveModel = m_scene.getUniqueActiveModel();
		if (m_uniqueActiveModel != nullptr)
		{
			std::string name = m_uniqueActiveModel->getName();
			std::copy(name.begin(), name.end(), m_name.begin());
			m_name[name.size()] = '\0';

			m_state = State::renaming;
			m_focusFirstTime = true;
		}
	}
}

void GUI::startRotatingX()
{
	if (m_state == State::none)
	{
		m_rotationDeg = 0;
		m_state = State::rotatingX;
		m_focusFirstTime = true;
	}
}

void GUI::startRotatingY()
{
	if (m_state == State::none)
	{
		m_rotationDeg = 0;
		m_state = State::rotatingY;
		m_focusFirstTime = true;
	}
}

void GUI::startRotatingZ()
{
	if (m_state == State::none)
	{
		m_rotationDeg = 0;
		m_state = State::rotatingZ;
		m_focusFirstTime = true;
	}
}

void GUI::startScalingX()
{
	if (m_state == State::none)
	{
		m_scale = 0;
		m_state = State::scalingX;
		m_focusFirstTime = true;
	}
}

void GUI::startScalingY()
{
	if (m_state == State::none)
	{
		m_scale = 0;
		m_state = State::scalingY;
		m_focusFirstTime = true;
	}
}

void GUI::startScalingZ()
{
	if (m_state == State::none)
	{
		m_scale = 0;
		m_state = State::scalingZ;
		m_focusFirstTime = true;
	}
}

void GUI::cancel()
{
	switch (m_state)
	{
	case State::renaming:
		m_uniqueActiveModel = nullptr;
		break;

	default:
		break;
	}

	m_state = State::none;
}

void GUI::apply()
{
	switch (m_state)
	{
	case State::none:
		break;

	case State::renaming:
		m_uniqueActiveModel->setName(std::string{m_name.data()});
		m_uniqueActiveModel = nullptr;
		break;

	case State::rotatingX:
		m_scene.getActiveModelsCenter().rotateX(glm::radians(m_rotationDeg));
		break;

	case State::rotatingY:
		m_scene.getActiveModelsCenter().rotateY(glm::radians(m_rotationDeg));
		break;

	case State::rotatingZ:
		m_scene.getActiveModelsCenter().rotateZ(glm::radians(m_rotationDeg));
		break;

	case State::scalingX:
		m_scene.getActiveModelsCenter().scaleX(m_scale);
		break;

	case State::scalingY:
		m_scene.getActiveModelsCenter().scaleY(m_scale);
		break;

	case State::scalingZ:
		m_scene.getActiveModelsCenter().scaleZ(m_scale);
		break;
	}

	m_state = State::none;
}

void GUI::deleteActiveModels()
{
	if (m_state == State::none)
	{
		m_scene.deleteActiveModels();
	}
}

void GUI::setWindowSize(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;
}

GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::getValues()
{
	m_renderMode = renderModeLabels[static_cast<int>(m_scene.getRenderMode())];
	m_cameraType = cameraTypeLabels[static_cast<int>(m_scene.getCameraType())];
}

void GUI::setValues()
{
	int renderModeIndex =
		static_cast<int>(std::find(renderModeLabels.begin(), renderModeLabels.end(), m_renderMode) -
		renderModeLabels.begin());
	m_scene.setRenderMode(static_cast<RenderMode>(renderModeIndex));
	int cameraTypeIndex =
		static_cast<int>(std::find(cameraTypeLabels.begin(), cameraTypeLabels.end(), m_cameraType) -
		cameraTypeLabels.begin());
	m_scene.setCameraType(static_cast<CameraType>(cameraTypeIndex));
}

void GUI::renderMode()
{
	if (ImGui::BeginCombo("render mode", m_renderMode.c_str()))
	{
		for (int renderModeIndex = 0; renderModeIndex < renderModeCount; ++renderModeIndex)
		{
			bool isSelected = m_renderMode == renderModeLabels[renderModeIndex];
			if (ImGui::Selectable(renderModeLabels[renderModeIndex].c_str(), isSelected))
			{
				m_renderMode = renderModeLabels[renderModeIndex];
			}
		}
		ImGui::EndCombo();
	}
}

void GUI::cameraType()
{
	if (ImGui::BeginCombo("camera type", m_cameraType.c_str()))
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
}

void GUI::camera()
{
	ImGui::Text("Camera");
	ImGui::Spacing();
	m_scene.getActiveCamera().getGUI().update();
}

void GUI::cursor()
{
	ImGui::Text("Cursor");
	ImGui::Spacing();
	m_scene.getCursor().getGUI().update();
}

void GUI::activeModelsCenter()
{
	ImGui::Text("Active models");
	ImGui::Spacing();
	m_scene.getActiveModelsCenter().getGUI().update();
}

void GUI::buttons()
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

	if (ImGui::Button("Add bezier curve"))
	{
		m_scene.addBezierCurve();
	}

	ImGui::Spacing();

	if (ImGui::Button("Add points to curve"))
	{
		m_scene.addActivePointsToCurve();
	}
}

void GUI::modelList()
{
	ImGui::Text("Model list");

	const std::vector<Model*>& models = m_scene.getModels();

	constexpr ImGuiTreeNodeFlags globalFlags =
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	std::optional<int> clickedId{};
	for (int i = 0; i < models.size(); ++i)
	{
		ImGuiTreeNodeFlags flags = globalFlags;
		if (models[i]->isActive()) flags |= ImGuiTreeNodeFlags_Selected;

		bool isOpen = ImGui::TreeNodeEx(
			(models[i]->getName() + "##modelList" + std::to_string(i)).c_str(), flags);
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

	if (clickedId.has_value() && m_state == State::none)
	{
		if (ImGui::GetIO().KeyCtrl)
		{
			m_scene.setModelIsActive(*clickedId, !models[*clickedId]->isActive());
		}
		else
		{
			for (int i = 0; i < models.size(); ++i)
			{
				m_scene.setModelIsActive(i, i == *clickedId);
			}
		}
	}
}

void GUI::separator()
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}
