#include "guis/cameraGUIs/orthographicCameraGUI.hpp"

#include "cameras/orthographicCamera.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>
#include <string>

OrthographicCameraGUI::OrthographicCameraGUI(OrthographicCamera& camera) :
	m_camera{camera}
{ }

void OrthographicCameraGUI::update()
{
	static const std::string suffix = "##orthographicCamera";

	getValues();

	ImGui::InputFloat(("view width" + suffix).c_str(), &m_viewWidth, 0.1f, 0.1f, "%.2f");
	m_viewWidth = std::max(m_viewWidth, 0.01f);

	setValues();
}

void OrthographicCameraGUI::getValues()
{
	m_viewWidth = m_camera.getViewWidth();
}

void OrthographicCameraGUI::setValues()
{
	m_camera.setViewWidth(m_viewWidth);
}
