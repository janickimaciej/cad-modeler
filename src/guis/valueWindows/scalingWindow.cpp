#include "guis/valueWindows/scalingWindow.hpp"

#include <imgui/imgui.h>

ScalingWindow::ScalingWindow(Scene& scene, const glm::ivec2& windowSize) :
	ValueWindow{windowSize},
	m_scene{scene}
{ }

void ScalingWindow::startScaling()
{
	m_rotationDeg = 0;
}

void ScalingWindow::apply(GUIMode mode)
{
	switch (mode)
	{
		case GUIMode::scalingX:
			m_scene.getActiveModelsCenter().scaleX(glm::radians(m_rotationDeg));
			break;

		case GUIMode::scalingY:
			m_scene.getActiveModelsCenter().scaleY(glm::radians(m_rotationDeg));
			break;

		case GUIMode::scalingZ:
			m_scene.getActiveModelsCenter().scaleZ(glm::radians(m_rotationDeg));
			break;
	}
}

std::string ScalingWindow::name()
{
	return "scaling";
}

int ScalingWindow::width()
{
	return 100;
}

void ScalingWindow::input()
{
	ImGui::InputFloat("##scaling", &m_rotationDeg, 1.0f, 1.0f, "%.2f");
}
