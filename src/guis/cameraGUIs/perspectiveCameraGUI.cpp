#include "guis/cameraGUIs/perspectiveCameraGUI.hpp"

#include "cameras/perspectiveCamera.hpp"

#include <imgui/imgui.h>

#include <algorithm>
#include <string>

PerspectiveCameraGUI::PerspectiveCameraGUI(PerspectiveCamera& camera) :
	m_camera{camera}
{ }

void PerspectiveCameraGUI::update()
{
	static const std::string suffix = "##perspectiveCamera";

	static constexpr int stepPrecision = 1;

	int fovYDeg = static_cast<int>(m_camera.getFOVYDeg());
	int prevFOVDeg = fovYDeg;
	ImGui::InputInt(("FOV Y" + suffix).c_str(), &fovYDeg, stepPrecision, stepPrecision);
	fovYDeg = std::clamp(fovYDeg, 20, 160);
	if (fovYDeg != prevFOVDeg)
	{
		m_camera.setFOVYDeg(static_cast<float>(fovYDeg));
	}
}
