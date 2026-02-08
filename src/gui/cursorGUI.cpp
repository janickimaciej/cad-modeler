#include "gui/cursorGUI.hpp"

#include "cursor.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>

CursorGUI::CursorGUI(Cursor& cursor) :
	m_cursor{cursor}
{ }

void CursorGUI::update(const Camera& camera)
{
	updatePos();
	updateViewportPos(camera);
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

void CursorGUI::updateViewportPos(const Camera& camera)
{
	static constexpr float stepPrecision = 5.0f;
	static const std::string format = "%.0f";

	glm::vec3 prevPos = m_cursor.getPos();
	glm::vec2 viewportPos = camera.posToViewportPos(prevPos);
	glm::vec2 prevViewportPos = viewportPos;
	ImGui::InputFloat(("viewport x" + m_suffix).c_str(), &viewportPos.x, stepPrecision,
		stepPrecision, format.c_str());
	ImGui::InputFloat(("viewport y" + m_suffix).c_str(), &viewportPos.y, stepPrecision,
		stepPrecision, format.c_str());
	if (viewportPos != prevViewportPos)
	{
		glm::vec3 pos = camera.viewportPosToPos(prevPos, viewportPos);
		m_cursor.setPos(pos);
	}
}
