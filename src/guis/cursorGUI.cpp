#include "guis/cursorGUI.hpp"

#include "cursor.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>

CursorGUI::CursorGUI(Cursor& cursor) :
	m_cursor{cursor}
{ }

void CursorGUI::update(const Camera& camera)
{
	updatePos();
	updateScreenPos(camera);
}

const std::string CursorGUI::m_suffix = "##cursor";

void CursorGUI::updatePos()
{
	static constexpr float stepPrecision = 0.02f;
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

void CursorGUI::updateScreenPos(const Camera& camera)
{
	static constexpr float stepPrecision = 5.0f;
	static const std::string format = "%.0f";

	glm::vec3 prevPos = m_cursor.getPos();
	glm::vec2 screenPos = camera.posToScreenPos(prevPos);
	glm::vec2 prevScreenPos = screenPos;
	ImGui::InputFloat(("screen x" + m_suffix).c_str(), &screenPos.x, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("screen y" + m_suffix).c_str(), &screenPos.y, stepPrecision, stepPrecision,
		format.c_str());
	if (screenPos != prevScreenPos)
	{
		glm::vec3 pos = camera.screenPosToPos(prevPos, screenPos);
		m_cursor.setPos(pos);
	}
}
