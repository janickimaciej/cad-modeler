#include "guis/model_guis/point_gui.hpp"

#include "models/point.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>
#include <string>

PointGUI::PointGUI(Point& point) :
	m_point{point}
{ }

void PointGUI::update()
{
	const std::string suffix = "##" + m_point.getOriginalName();

	getValues();

	ImGui::InputFloat(("x" + suffix).c_str(), &m_x, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("y" + suffix).c_str(), &m_y, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("z" + suffix).c_str(), &m_z, 0.1f, 0.1f, "%.2f");

	setValues();
}

void PointGUI::getValues()
{
	glm::vec3 position = m_point.getPosition();
	m_x = position.x;
	m_y = position.y;
	m_z = position.z;
}

void PointGUI::setValues()
{
	m_point.setPosition(glm::vec3{m_x, m_y, m_z});
}
