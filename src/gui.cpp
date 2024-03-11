#include "gui.hpp"

#include "camera_type.hpp"
#include "render_mode.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>

GUI::GUI(GLFWwindow* window, Scene& scene) :
	m_scene{scene}
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
	
	ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
	constexpr int guiWidth = 250;
	constexpr int guiHeight = 1000;
	ImGui::SetNextWindowSize({guiWidth, guiHeight}, ImGuiCond_Always);
	ImGui::Begin("GUI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBackground);
	ImGui::PushItemWidth(150);

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
	
	separator();

	m_scene.getActiveCamera().getGUI().update();

	separator();

	m_scene.getActiveModel().getGUI().update();

	ImGui::End();

	setValues();
}

void GUI::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void GUI::separator()
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}
