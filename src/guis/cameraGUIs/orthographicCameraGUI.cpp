#include "guis/cameraGUIs/orthographicCameraGUI.hpp"

#include "cameras/orthographicCamera.hpp"

#include <imgui/imgui.h>

#include <algorithm>
#include <string>

OrthographicCameraGUI::OrthographicCameraGUI(OrthographicCamera& camera) :
	m_camera{camera}
{ }

void OrthographicCameraGUI::update()
{
	static const std::string suffix = "##orthographicCamera";

	float viewWidth = m_camera.getViewWidth();
	ImGui::InputFloat(("view width" + suffix).c_str(), &viewWidth, 0.1f, 0.1f, "%.1f");
	viewWidth = std::max(viewWidth, 0.1f);
	m_camera.setViewWidth(viewWidth);
}
