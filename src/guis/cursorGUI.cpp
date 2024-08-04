#include "guis/cursorGUI.hpp"

#include "cursor.hpp"

#include <imgui/imgui.h>

CursorGUI::CursorGUI(Cursor& cursor) :
	m_cursor{cursor}
{ }

void CursorGUI::update(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize)
{
	updatePos();
	updateScreenPos(cameraMatrix, windowSize);
}

const std::string CursorGUI::m_suffix = "##cursor";

void CursorGUI::updatePos()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";

	glm::vec3 pos = m_cursor.getPos();
	glm::vec3 prevPos = pos;
	ImGui::InputFloat(("x" + m_suffix).c_str(), &pos.x, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("y" + m_suffix).c_str(), &pos.y, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("z" + m_suffix).c_str(), &pos.z, stepPrecision, stepPrecision,
		format.c_str());
	if (pos != prevPos)
	{
		m_cursor.setPos(pos);
	}
}

void CursorGUI::updateScreenPos(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize)
{
	static constexpr float stepPrecision = 5.0f;
	static const std::string format = "%.0f";

	glm::vec2 screenPos = m_cursor.getScreenPos(cameraMatrix, windowSize);
	glm::vec2 prevScreenPos = screenPos;
	ImGui::InputFloat(("screen x" + m_suffix).c_str(), &screenPos.x, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("screen y" + m_suffix).c_str(), &screenPos.y, stepPrecision, stepPrecision,
		format.c_str());
	if (screenPos != prevScreenPos)
	{
		m_cursor.setScreenPos(screenPos, cameraMatrix, windowSize);
	}
}
