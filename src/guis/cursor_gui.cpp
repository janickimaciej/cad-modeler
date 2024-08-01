#include "guis/cursor_gui.hpp"

#include "cursor.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <string>

CursorGUI::CursorGUI(Cursor& cursor) :
	m_cursor{cursor}
{ }

void CursorGUI::update(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize)
{
	static const std::string suffix = "##cursor";

	getValues(cameraMatrix, windowSize);

	ImGui::InputFloat(("x" + suffix).c_str(), &m_x, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("y" + suffix).c_str(), &m_y, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("z" + suffix).c_str(), &m_z, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("screen x" + suffix).c_str(), &m_screenX, 5.0f, 5.0f, "%.0f");

	ImGui::InputFloat(("screen y" + suffix).c_str(), &m_screenY, 5.0f, 5.0f, "%.0f");

	setValues(cameraMatrix, windowSize);
}

void CursorGUI::getValues(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize)
{
	glm::vec3 position = m_cursor.getPosition();
	m_x = position.x;
	m_prevX = m_x;
	m_y = position.y;
	m_prevY = m_y;
	m_z = position.z;
	m_prevZ = m_z;
	glm::vec2 screenPosition = m_cursor.getScreenPosition(cameraMatrix, windowSize);
	m_screenX = screenPosition.x;
	m_prevScreenX = m_screenX;
	m_screenY = screenPosition.y;
	m_prevScreenY = m_screenY;
}

void CursorGUI::setValues(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize)
{
	if (m_x != m_prevX || m_y != m_prevY || m_z != m_prevZ)
	{
		m_cursor.setPosition({m_x, m_y, m_z});
	}
	if (m_screenX != m_prevScreenX || m_screenY != m_prevScreenY)
	{
		m_cursor.setScreenPosition({m_screenX, m_screenY}, cameraMatrix, windowSize);
	}
}
