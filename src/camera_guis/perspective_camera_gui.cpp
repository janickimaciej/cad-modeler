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

	ImGui::InputInt("FOV", &m_fovDeg, 1, 1);
	m_fovDeg = std::clamp(m_fovDeg, 1, 179);

	setValues();
}

void PerspectiveCameraGUI::getValues()
{
	m_fovDeg = m_camera.getFOVDeg();
}

void PerspectiveCameraGUI::setValues()
{
	m_camera.setFOVDeg(m_fovDeg);
}
