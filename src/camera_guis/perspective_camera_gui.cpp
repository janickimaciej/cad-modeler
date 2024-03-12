#include "camera_guis/perspective_camera_gui.hpp"

#include "cameras/perspective_camera.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>

PerspectiveCameraGUI::PerspectiveCameraGUI(PerspectiveCamera& camera) :
	m_camera{camera}
{ }

void PerspectiveCameraGUI::update()
{
	getValues();

	ImGui::InputInt("FOV Y", &m_fovYDeg, 1, 1);
	m_fovYDeg = std::clamp(m_fovYDeg, 1, 179);

	setValues();
}

void PerspectiveCameraGUI::getValues()
{
	m_fovYDeg = m_camera.getFOVYDeg();
}

void PerspectiveCameraGUI::setValues()
{
	m_camera.setFOVYDeg(m_fovYDeg);
}
