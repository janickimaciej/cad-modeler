#include "guis/center_point_gui.hpp"

#include "center_point.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <string>

CenterPointGUI::CenterPointGUI(CenterPoint& centerPoint) :
	m_centerPoint{centerPoint}
{ }

void CenterPointGUI::update()
{
	const std::string suffix = "##centerPoint";

	getValues();

	ImGui::InputFloat(("x" + suffix).c_str(), &m_x, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("y" + suffix).c_str(), &m_y, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("z" + suffix).c_str(), &m_z, 0.1f, 0.1f, "%.2f");

	setValues();
}

void CenterPointGUI::getValues()
{
	glm::vec3 position = m_centerPoint.getPosition();
	m_x = position.x;
	m_y = position.y;
	m_z = position.z;
}

void CenterPointGUI::setValues()
{
	m_centerPoint.setPosition(glm::vec3{m_x, m_y, m_z});
}
