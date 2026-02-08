#include "gui/cameraGUIs/orthographicCameraGUI.hpp"

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

	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.1f";

	float viewHeight = m_camera.getViewHeight();
	float prevViewHeight = viewHeight;
	ImGui::InputFloat(("view height" + suffix).c_str(), &viewHeight, stepPrecision, stepPrecision,
		format.c_str());
	viewHeight = std::max(viewHeight, 0.1f);
	if (viewHeight != prevViewHeight)
	{
		m_camera.setViewHeight(viewHeight);
	}
}
