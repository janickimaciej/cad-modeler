#include "guis/valueWindows/rotatingWindow.hpp"

#include <imgui/imgui.h>

RotatingWindow::RotatingWindow(Scene& scene, const glm::ivec2& viewportSize) :
	ValueWindow{viewportSize},
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
			m_scene.rotateXSelectedModels(glm::radians(m_rotationDeg));
			break;

		case GUIMode::rotatingY:
			m_scene.rotateYSelectedModels(glm::radians(m_rotationDeg));
			break;

		case GUIMode::rotatingZ:
			m_scene.rotateZSelectedModels(glm::radians(m_rotationDeg));
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
