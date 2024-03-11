#include "camera_guis/orthographic_camera_gui.hpp"

#include "cameras/orthographic_camera.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>

OrthographicCameraGUI::OrthographicCameraGUI(OrthographicCamera& camera) :
	m_camera{camera}
{ }

void OrthographicCameraGUI::update()
{
	getValues();

	ImGui::InputFloat("view width", &m_viewWidth, 0.1f, 0.1f, "%.2f");
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
