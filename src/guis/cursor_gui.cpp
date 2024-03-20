#include "guis/cursor_gui.hpp"

#include "cursor.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <string>

CursorGUI::CursorGUI(Cursor& cursor) :
	m_cursor{cursor}
{ }

void CursorGUI::update()
{
	const std::string suffix = "##cursor";

	getValues();

	ImGui::InputFloat(("x" + suffix).c_str(), &m_x, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("y" + suffix).c_str(), &m_y, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("z" + suffix).c_str(), &m_z, 0.1f, 0.1f, "%.2f");

	setValues();
}

void CursorGUI::getValues()
{
	glm::vec3 position = m_cursor.getPosition();
	m_x = position.x;
	m_y = position.y;
	m_z = position.z;
}

void CursorGUI::setValues()
{
	m_cursor.setPosition(glm::vec3{m_x, m_y, m_z});
}
