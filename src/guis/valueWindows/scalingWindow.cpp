#include "guis/valueWindows/scalingWindow.hpp"

#include <imgui/imgui.h>

ScalingWindow::ScalingWindow(Scene& scene, const glm::ivec2& windowSize) :
	ValueWindow{windowSize},
	m_scene{scene}
{ }

void ScalingWindow::startScaling()
{
	m_scale = 0;
}

void ScalingWindow::apply(GUIMode mode)
{
	switch (mode)
	{
		case GUIMode::scalingX:
			m_scene.scaleXSelectedModels(m_scale);
			break;

		case GUIMode::scalingY:
			m_scene.scaleYSelectedModels(m_scale);
			break;

		case GUIMode::scalingZ:
			m_scene.scaleZSelectedModels(m_scale);
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
	ImGui::InputFloat("##scaling", &m_scale, 1.0f, 1.0f, "%.2f");
}
