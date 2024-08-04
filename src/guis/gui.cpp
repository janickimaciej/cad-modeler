#include "guis/gui.hpp"

#include "cameraType.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>
#include <optional>

GUI::GUI(GLFWwindow* window, Scene& scene, const glm::ivec2& windowSize) :
	m_leftPanel{scene, windowSize},
	m_rotatingWindow{scene, windowSize},
	m_scalingWindow{scene, windowSize},
	m_renamingWindow{scene, windowSize},
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

GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::update()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	switch (m_mode)
	{
		case GUIMode::rotatingX:
		case GUIMode::rotatingY:
		case GUIMode::rotatingZ:
			m_rotatingWindow.update();
			break;

		case GUIMode::scalingX:
		case GUIMode::scalingY:
		case GUIMode::scalingZ:
			m_scalingWindow.update();
			break;

		case GUIMode::renaming:
			m_renamingWindow.update();
			break;
	}

	m_leftPanel.update(m_mode);
}

void GUI::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::startRotatingX()
{
	startRotating(GUIMode::rotatingX);
}

void GUI::startRotatingY()
{
	startRotating(GUIMode::rotatingY);
}

void GUI::startRotatingZ()
{
	startRotating(GUIMode::rotatingZ);
}

void GUI::startScalingX()
{
	startScaling(GUIMode::scalingX);
}

void GUI::startScalingY()
{
	startScaling(GUIMode::scalingY);
}

void GUI::startScalingZ()
{
	startScaling(GUIMode::scalingZ);
}

void GUI::startRenaming()
{
	if (m_mode == GUIMode::none && m_renamingWindow.startRenaming())
	{
		m_mode = GUIMode::renaming;
	}
}

void GUI::cancel()
{
	switch (m_mode)
	{
		case GUIMode::renaming:
			m_renamingWindow.cancel();
			break;
	}

	m_mode = GUIMode::none;
}

void GUI::apply()
{
	switch (m_mode)
	{
		case GUIMode::rotatingX:
		case GUIMode::rotatingY:
		case GUIMode::rotatingZ:
			m_rotatingWindow.apply(m_mode);
			break;

		case GUIMode::scalingX:
		case GUIMode::scalingY:
		case GUIMode::scalingZ:
			m_scalingWindow.apply(m_mode);
			break;

		case GUIMode::renaming:
			m_renamingWindow.apply();
			break;
	}

	m_mode = GUIMode::none;
}

void GUI::deleteActiveModels()
{
	if (m_mode == GUIMode::none)
	{
		m_scene.deleteActiveModels();
	}
}

void GUI::startRotating(GUIMode mode)
{
	if (m_mode == GUIMode::none)
	{
		m_mode = mode;
		m_rotatingWindow.startRotating();
	}
}

void GUI::startScaling(GUIMode mode)
{
	if (m_mode == GUIMode::none)
	{
		m_mode = mode;
		m_scalingWindow.startScaling();
	}
}
