#include "guis/valueWindows/rotatingWindow.hpp"

#include <imgui/imgui.h>

RotatingWindow::RotatingWindow(Scene& scene, const glm::ivec2& windowSize) :
	ValueWindow{windowSize},
	m_scene{scene}
{ }

void RotatingWindow::startRotating()
{
	m_rotationDeg = 0;
}

void RotatingWindow::apply(GUIMode mode)
{
	switch (mode)
	{
		case GUIMode::rotatingX:
			m_scene.getActiveModelsCenter().rotateX(glm::radians(m_rotationDeg));
			break;

		case GUIMode::rotatingY:
			m_scene.getActiveModelsCenter().rotateY(glm::radians(m_rotationDeg));
			break;

		case GUIMode::rotatingZ:
			m_scene.getActiveModelsCenter().rotateZ(glm::radians(m_rotationDeg));
			break;
	}
}

std::string RotatingWindow::name()
{
	return "rotating";
}

int RotatingWindow::width()
{
	return 100;
}

void RotatingWindow::input()
{
	ImGui::InputFloat("##rotating", &m_rotationDeg, 1.0f, 1.0f, "%.2f");
}
